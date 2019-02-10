/*!
 * \file game.h contains the main game wrapper with the game loop and state management
 */
#pragma once

#include "states/state_manager.h"

#include <vector>
#include <memory>
#include <string_view>

#include <glm/vec2.hpp>

struct GLFWwindow;

namespace pac
{
/*!
 * \brief The Game class is the highest level wrapper around the game state. It keeps track of the active
 * state, and delegates work to the various systems that need to work together.
 */
class Game
{
private:
    /* The Game Window */
    GLFWwindow* m_window = nullptr;

    /* The currently active game state */
    StateManager m_state_manager = {};

    /* This struct will contain flags that can be flipped on / off to toggle features */
    struct Flags
    {
        uint8_t running = true;
    } m_flags = {true};

public:
    Game(std::string_view title, glm::uvec2 window_size);

    ~Game() noexcept;

    void run();

private:
    /*!
     * \brief init_glfw_window initializes the game window and ensures there is an active OpenGL Context
     * \param title is the title of the window
     * \param window_size is the desired size of the window
     */
    void init_glfw_window(const char* title, glm::uvec2 window_size);

    /*!
     * \brief init_imgui initializes ImGui
     */
    void init_imgui();

    /*!
     * \brief update updates the game state
     * \param dt is the delta time since the previous update
     */
    void update(float dt);

    /*!
     * \brief render will render what needs to be rendered.
     */
    void draw();
};

}  // namespace pac
