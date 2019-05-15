#include "game.h"
#include "common.h"
#include "input/input.h"
#include "states/game_state.h"
#include "states/main_menu_state.h"
#include "rendering/shader_program.h"
#include "rendering/renderer.h"
#include "audio/sound_manager.h"

#include <chrono>

#include <gfx.h>
#include <cglutil.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <entt/signal/dispatcher.hpp>

namespace pac
{
/* The game event queue */
entt::dispatcher g_event_queue{};

Game::Game(std::string_view title, glm::uvec2 window_size)
{
    init_glfw_window(title.data(), window_size);
    init_imgui();
    set_up_lua();
}

Game::~Game() noexcept
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_window);
}

void Game::run()
{
    /* Add initial state to the stack */
    m_state_manager.push<MainMenuState>({&m_state_manager, &m_lua, &m_registry});

    /* Create variables for tracking frame-times */
    std::chrono::steady_clock delta_clock = {};
    auto last_frame = delta_clock.now();

    do
    {
        /* Set ImGui up for a new frame */
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        /* Compute delta time in floating point seconds */
        const float dt = std::chrono::duration<float>(delta_clock.now() - last_frame).count();
        last_frame = delta_clock.now();

        /* ImGui Stuff */
#ifndef NDEBUG
        ImGui::Text("FPS: %5.1f", ImGui::GetIO().Framerate);
        ImGui::SameLine(0.f, 25.f);
        ImGui::Text("Frame Time: %6.4fms", dt * 1000.f);
#endif

        glfwPollEvents();
        g_event_queue.update();
        update(dt);
        draw();
    } while (m_flags.running && !glfwWindowShouldClose(m_window) && !m_state_manager.empty());
}

void Game::init_glfw_window(const char* title, glm::uvec2 window_size)
{
    /* Request an OpenGL 4.5 Core Profile */
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* In Debug mode, make the GL context a Debug context so we can use debug callback for errors (further
     * down) */
#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

    m_window = glfwCreateWindow(window_size.x, window_size.y, title, nullptr, nullptr);
    GFX_ASSERT(m_window, "Failed to create window %ux%u", window_size.x, window_size.y);

    glfwSetCursorPosCallback(m_window, pac::cursor_position_callback);
    glfwSetMouseButtonCallback(m_window, pac::mouse_button_callback);
    glfwSetKeyCallback(m_window, pac::key_callback);
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(0);

    /* Loads all GL function pointers for OpenGL 4.5 Core */
    gladLoadGL();

    /* Ensure we got a 4.5 Profile */
    int v_maj = 0;
    int v_min = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &v_maj);
    glGetIntegerv(GL_MINOR_VERSION, &v_min);

    GFX_ASSERT(v_maj >= 4 && v_min >= 5, "The created context does not support OpenGL 4.5, so we must fail!");

    /* Enable default debug callback provided by cgl */
#ifndef NDEBUG
//    cgl::create_debug_callback();
#endif
}

void Game::init_imgui()
{
    ImGui::CreateContext();

    /* Modify style to match our application, starting with default color style */
    ImGui::StyleColorsDark();
    auto& style = ImGui::GetStyle();

    /* Update rounding */
    style.WindowRounding = 0.f;
    style.FrameRounding = 0.f;
    style.ScrollbarRounding = 0.f;
    style.TabRounding = 0.f;

    /* Update Colors */
    auto* colors = style.Colors;
    colors[ImGuiCol_Button] = ImVec4(0.01f, 0.00f, 0.78f, 0.40f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.02f, 0.00f, 1.00f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 0.10f, 1.00f, 1.00f);

    /* Load Font */
    ImGuiIO& io = ImGui::GetIO();
    const auto abs_path = cgl::native_absolute_path("res/ATI_9x16.ttf");
    io.Fonts->AddFontFromFileTTF(abs_path.c_str(), 16.f);

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init();
}

void Game::update(float dt)
{
    get_input().update(dt, m_window);
    m_state_manager.update(dt);
}

void Game::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_state_manager.draw();
    get_renderer().submit_work();

    glfwSwapBuffers(m_window);
}

void Game::set_up_lua()
{
    m_lua.open_libraries(sol::lib::base, sol::lib::package);

    /* Register Data Types */

    /* Register action enum */
    m_lua.new_enum<Action>("Action", {{"MOVE_NORTH", ACTION_MOVE_NORTH},
                                      {"MOVE_EAST", ACTION_MOVE_EAST},
                                      {"MOVE_SOUTH", ACTION_MOVE_SOUTH},
                                      {"MOVE_WEST", ACTION_MOVE_WEST}});

    /* Create action functions (each of these requires a certain component to work. It is the callers responsibility that the
     * given entity has this component. This makes for a flexible way to tell something what you want to do */
    m_lua.set_function("move", [this](unsigned e, int x, int y) { m_registry.get<CMovement>(e).desired_direction = {x, y}; });

    /* Animation actions */
    m_lua.set_function("set_animation", [this](unsigned e, const std::string& anim) {
        auto& ac = m_registry.get<CAnimationSprite>(e);
        ac.active_animation = ac.available_animations.at(anim);
    });
}

}  // namespace pac
