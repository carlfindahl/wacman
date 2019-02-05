#include "game.h"
#include "input.h"

#include <chrono>

#include <gfx.h>
#include <GLFW/glfw3.h>

namespace pac
{
Game::Game(const char* title, glm::uvec2 window_size) { init_glfw_window(title, window_size); }

Game::~Game() noexcept { glfwDestroyWindow(m_window); }

void Game::run()
{
    /* Create variables for tracking frame-times */
    std::chrono::steady_clock delta_clock = {};
    auto last_frame = delta_clock.now();

    while (m_flags.running && !glfwWindowShouldClose(m_window))
    {
        /* Compute delta time in floating point seconds */
        const float dt = std::chrono::duration<float>(delta_clock.now() - last_frame).count();
        last_frame = delta_clock.now();

        glfwPollEvents();
        update(dt);
        render();
    }
}

void Game::init_glfw_window(const char* title, glm::uvec2 window_size)
{
    /* Request an OpenGL 4.5 Core Profile */
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(window_size.x, window_size.y, title, nullptr, nullptr);
    GFX_ASSERT(m_window, "Failed to create window %ux%u", window_size.x, window_size.y);

    glfwSetCursorPosCallback(m_window, pac::input::cursor_position_callback);
    glfwSetMouseButtonCallback(m_window, pac::input::mouse_button_callback);
    glfwSetKeyCallback(m_window, pac::input::key_callback);
    glfwMakeContextCurrent(m_window);

    /* Loads all GL function pointers for OpenGL 4.5 Core */
    gladLoadGL();
}

void Game::update(float dt) {}

void Game::render() { glfwSwapBuffers(m_window); }

}  // namespace pac
