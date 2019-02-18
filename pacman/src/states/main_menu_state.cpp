#include "main_menu_state.h"

#include <imgui/imgui.h>

namespace pac
{
void pac::MainMenuState::on_enter() {}

void pac::MainMenuState::on_exit() {}

bool pac::MainMenuState::update(float dt)
{
    ImGui::Begin("Main Menu");

    if (ImGui::Button("Start Game")) {}

    if (ImGui::Button("High Scores")) {}

    if (ImGui::Button("Exit")) {

    }

    ImGui::End();
    return false;
}

bool pac::MainMenuState::draw() { return true; }

}  // namespace pac
