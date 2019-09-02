/*!
 * \file game.h contains the main game wrapper with the game loop and state management
 */
#pragma once

#include "states/state_manager.h"
#include "reflect.h"

#include <vector>
#include <memory>
#include <string_view>

#include <glm/vec2.hpp>
#include <sol/state.hpp>
#include <entt/entity/registry.hpp>
#include <robinhood/robinhood.h>

namespace pac
{
/*!
 * \brief The Game class is the highest level wrapper around the game state. It keeps track of the active
 * state, and delegates work to the various systems that need to work together.
 */
class Game
{
private:
    /* The currently active game state */
    StateManager m_state_manager = {};

    /* Lua State */
    sol::state m_lua{};

    /* Entity Registry */
    entt::registry m_registry{};

    /* The Game Window */
    struct GLFWwindow* m_window = nullptr;

    /* Event Binder to allow lua to bind events */
    robin_hood::unordered_map<std::string, entt::scoped_connection (*)(entt::dispatcher&, sol::function&)> m_lua_events{};

    /* To keep objects alive (hacky solution until I find a better way to hook up lua with entt events) */
    std::vector<sol::function> m_registered_event_functions{};

    /* This struct will contain flags that can be flipped on / off to toggle features */
    struct Flags
    {
        uint8_t running = true;
    } m_flags = {true};

public:
    Game(std::string_view title, glm::uvec2 window_size);

    Game(const Game&) = delete;

    Game(Game&&) = delete;

    Game& operator=(const Game&) = delete;
    Game& operator=(Game&&) = delete;

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

private:
    /*!
     * \brief set_up_lua adds lua bindings and functions to the lua state
     */
    void set_up_lua();
};

}  // namespace pac
