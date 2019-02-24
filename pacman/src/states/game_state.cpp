#include "game_state.h"
#include "audio/sound_manager.h"
#include "state_manager.h"
#include "pause_state.h"
#include "input.h"
#include "config.h"

#include <gfx.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace pac
{
GameState::GameState(GameContext owner) : State(owner), m_level(owner) {}

void GameState::on_enter()
{
    m_music_id = get_sound().play("theme", true);

    m_overlay = get_renderer().load_texture("res/ingame_overlay.png");

    input::InputState game_input(true);
    game_input.set_binding(GLFW_KEY_ESCAPE, [this] { m_context.state_manager->push<PauseState>(m_context); });
    game_input.set_binding(GLFW_KEY_P, [this] { m_context.state_manager->push<PauseState>(m_context); });

    auto& input_manager = input::get_input();
    input_manager.push(std::move(game_input));

    m_level.load("res/level0");
}

void GameState::on_exit()
{
    get_sound().stop(m_music_id);
    input::get_input().pop();
}

bool GameState::update(float dt)
{
    m_level.update(dt);
    return false;
}

bool GameState::draw()
{
    m_level.draw();
    get_renderer().draw({{SCREEN_W / 2.f, SCREEN_H / 2.f}, glm::vec2(SCREEN_W, SCREEN_H), {1.f, 1.f, 1.f}, m_overlay});
    return false;
}
}  // namespace pac
