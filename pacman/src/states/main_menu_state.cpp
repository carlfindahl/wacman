#include "main_menu_state.h"
#include "game_state.h"
#include "state_manager.h"

#include <gfx.h>
#include <imgui/imgui.h>

namespace pac
{
void pac::MainMenuState::on_enter() {}

void pac::MainMenuState::on_exit() {}

bool pac::MainMenuState::update(float dt)
{
    ImGui::Begin("Main Menu");

    if (ImGui::Button("Start Game", {150, 50}))
    {
        m_owner->push<GameState>();
    }

    if (ImGui::Button("High Scores", {150, 50}))
    {
        GFX_INFO("I will start high score screen.");
    }

    if (ImGui::Button("Exit", {150, 50}))
    {
        m_owner->pop();
    }

    ImGui::End();
    return false;
}

bool pac::MainMenuState::draw() { return true; }

}  // namespace pac
