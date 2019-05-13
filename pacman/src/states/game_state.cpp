#include "game_state.h"
#include "entity/components.h"
#include "audio/sound_manager.h"
#include "entity/input_system.h"
#include "entity/movement_system.h"
#include "entity/rendering_system.h"
#include "entity/animation_system.h"
#include "state_manager.h"
#include "pause_state.h"
#include "input/input.h"
#include "config.h"

#include <gfx.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <entt/signal/dispatcher.hpp>

namespace pac
{
extern entt::dispatcher g_event_queue;

GameState::GameState(GameContext owner) : State(owner), m_level(owner) {}

void GameState::on_enter()
{
    add_systems();

    m_lua.open_libraries(sol::lib::base, sol::lib::package);

    m_music_id = get_sound().play("theme", true);

    m_overlay = get_renderer().load_texture("res/ingame_overlay.png");

    m_level.load("res/level0");

    /* Create the input domain for the game */
    InputDomain game_input(true);
    game_input.bind_key(GLFW_KEY_ESCAPE, ACTION_BACK);
    game_input.bind_key(GLFW_KEY_P, ACTION_PAUSE);
    game_input.bind_key(GLFW_KEY_UP, ACTION_MOVE_NORTH);
    game_input.bind_key(GLFW_KEY_RIGHT, ACTION_MOVE_EAST);
    game_input.bind_key(GLFW_KEY_DOWN, ACTION_MOVE_SOUTH);
    game_input.bind_key(GLFW_KEY_LEFT, ACTION_MOVE_WEST);
    get_input().push(std::move(game_input));

    g_event_queue.sink<EvInput>().connect<&GameState::recieve>(this);

    /* Register action enum */
    m_lua.new_enum<Action>("Action", {{"MOVE_NORTH", ACTION_MOVE_NORTH},
                                      {"MOVE_EAST", ACTION_MOVE_EAST},
                                      {"MOVE_SOUTH", ACTION_MOVE_SOUTH},
                                      {"MOVE_WEST", ACTION_MOVE_WEST}});

    m_factory.spawn(m_lua, "food");
    m_factory.spawn(m_lua, "pacman");
}

void GameState::on_exit()
{
    g_event_queue.sink<EvInput>().disconnect<&GameState::recieve>(this);
    get_sound().stop(m_music_id);
    get_input().pop();
}

bool GameState::update(float dt)
{
    m_level.update(dt);
    for (auto& system : m_systems)
    {
        system->update(dt, m_registry);
    }
    return false;
}

bool GameState::draw()
{
    m_level.draw();
    get_renderer().draw({{SCREEN_W / 2.f, SCREEN_H / 2.f}, glm::vec2(SCREEN_W, SCREEN_H), {1.f, 1.f, 1.f}, m_overlay});
    return false;
}

void GameState::add_systems()
{
    m_systems.emplace_back(std::make_unique<InputSystem>());
    m_systems.emplace_back(std::make_unique<MovementSystem>(m_level));
    m_systems.emplace_back(std::make_unique<AnimationSystem>());
    m_systems.emplace_back(std::make_unique<RenderingSystem>());
}

void GameState::recieve(const EvInput& input)
{
    switch (input.action)
    {
    case ACTION_BACK: m_context.state_manager->pop(); break;
    case ACTION_PAUSE: m_context.state_manager->push<PauseState>(m_context); break;
    default: break;
    }
}
}  // namespace pac
