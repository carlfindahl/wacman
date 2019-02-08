#include "game_state.h"
#include <rendering/renderer.h>

#include <gfx.h>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace pac
{
GameState::GameState() = default;

GameState::~GameState() noexcept = default;

void GameState::on_enter()
{
    GFX_INFO("Entering GameState");
    m_ubo.update(glm::ortho(-400.f, 400.f, -400.f, 400.f), glm::mat4(1.f));
}

void GameState::on_exit() { GFX_INFO("Exiting GameState"); }

bool GameState::update(float dt) { return false; }

bool GameState::draw()
{
    m_ubo.bind(0u);

    decltype(auto) r = get_renderer();
    r.draw({{0.f, 0.f}, {100.f, 100.f}, {1.f, 0.f, 0.f}, 0u});

    return false;
}
}  // namespace pac
