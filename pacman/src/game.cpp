#include "game.h"
#include "input.h"
#include "states/game_state.h"
#include "rendering/shader_program.h"
#include "rendering/renderer.h"

#include <chrono>

#include <gfx.h>
#include <cglutil.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

namespace pac
{
Game::Game(const char* title, glm::uvec2 window_size)
{
    init_glfw_window(title, window_size);
    init_imgui();
    m_state_manager.push(std::make_unique<GameState>());
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
    /* Create variables for tracking frame-times */
    std::chrono::steady_clock delta_clock = {};
    auto last_frame = delta_clock.now();

    ShaderProgram prog({{GL_VERTEX_SHADER, "res/sprite.vert"}, {GL_FRAGMENT_SHADER, "res/sprite.frag"}});
    prog.use();

    while (m_flags.running && !glfwWindowShouldClose(m_window))
    {
        /* Set ImGui up for a new frame */
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        /* Compute delta time in floating point seconds */
        const float dt = std::chrono::duration<float>(delta_clock.now() - last_frame).count();
        last_frame = delta_clock.now();

        ImGui::Text("FPS: %5.1f", ImGui::GetIO().Framerate);
        ImGui::SameLine(0.f, 25.f);
        ImGui::Text("Frame Time: %6.4fms", dt);

        Gfx::IntrospectShader("Sprite Shader", prog);

        glfwPollEvents();
        update(dt);
        draw();
    }
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

    glfwSetCursorPosCallback(m_window, pac::input::cursor_position_callback);
    glfwSetMouseButtonCallback(m_window, pac::input::mouse_button_callback);
    glfwSetKeyCallback(m_window, pac::input::key_callback);
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(0);

    /* Loads all GL function pointers for OpenGL 4.5 Core */
    gladLoadGL();

    /* Enable default debug callback provided by cgl */
#ifndef NDEBUG
    cgl::create_debug_callback();
#endif
}

void Game::init_imgui()
{
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init();
}

void Game::update(float dt)
{
    m_state_manager.update(dt);
    glfwSetWindowUserPointer(m_window, m_state_manager.get_active_state());
}

void Game::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_state_manager.draw();
    get_renderer().submit_work();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(m_window);
}

}  // namespace pac
