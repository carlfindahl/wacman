#include "main_menu_state.h"
#include "game_state.h"
#include "high_score_state.h"
#include "respawn_state.h"
#include "state_manager.h"
#include "config.h"

#include <gfx.h>
#include <imgui/imgui.h>

namespace pac
{
void pac::MainMenuState::on_enter() { m_splash_texture = get_renderer().load_texture("res/splash_screen.png"); }

void pac::MainMenuState::on_exit() {}

bool pac::MainMenuState::update(float dt)
{
    /* Begin Main Menu Window, and center it */
    ImGui::SetNextWindowPos({SCREEN_W / 2.f, SCREEN_H / 2.f}, 0, {0.5f, 0.5f});
    ImGui::Begin("Main Menu", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

    if (ImGui::Button("Start Game", {150, 50}))
    {
        m_context.state_manager->push<GameState>(m_context);
        m_context.state_manager->push<RespawnState>(m_context);
    }

    if (ImGui::Button("High Scores", {150, 50}))
    {
        m_context.state_manager->push<HighScoreState>(m_context);
    }

    if (ImGui::Button("Exit", {150, 50}))
    {
        m_context.state_manager->pop();
    }

    ImGui::End();

    return false;
}

bool pac::MainMenuState::draw()
{
    get_renderer().draw({{SCREEN_W / 2.f, SCREEN_H / 2.f}, glm::vec2(SCREEN_W, SCREEN_H), {1.f, 1.f, 1.f}, m_splash_texture});
    return true;
}

}  // namespace pac
