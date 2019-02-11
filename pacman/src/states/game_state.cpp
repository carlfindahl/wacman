#include "game_state.h"
#include <config.h>

#include <gfx.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace pac
{
GameState::GameState() = default;

GameState::~GameState() noexcept = default;

void GameState::on_enter()
{
    GFX_INFO("Entering GameState");
    m_ubo.update(glm::ortho(0.f, 700.f, 900.f, 0.f), glm::mat4(1.f));
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
    m_ubo.bind(0u);
    m_level.draw();
    return false;
}
}  // namespace pac
