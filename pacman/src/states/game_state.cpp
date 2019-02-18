#include "game_state.h"
#include <config.h>

#include <gfx.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace pac
{
GameState::~GameState() noexcept = default;

void GameState::on_enter()
{
    GFX_INFO("Entering GameState");
    m_level.load("res/level0");
}

void GameState::on_exit() { GFX_INFO("Exiting GameState"); }

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
