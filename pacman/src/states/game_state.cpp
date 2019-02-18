#include "game_state.h"
#include "state_manager.h"
#include "pause_state.h"
#include "input.h"
#include "config.h"

#include <gfx.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace pac
{
GameState::~GameState() noexcept = default;

GameState::GameState(StateManager& owner) : State(owner) {}

void GameState::on_enter()
{
    GFX_INFO("Entering GameState");

    input::InputState game_input(true);
    game_input.set_binding(GLFW_KEY_ESCAPE, [this] { m_owner->push<PauseState>(); });

    auto& input_manager = input::get_input();
    input_manager.push(std::move(game_input));

    m_level.load("res/level0");
}

void GameState::on_exit() { input::get_input().pop(); }

bool GameState::update(float dt)
{
    m_level.update(dt);
    return false;
}

bool GameState::draw()
{
    m_level.draw();
    return false;
}
}  // namespace pac
