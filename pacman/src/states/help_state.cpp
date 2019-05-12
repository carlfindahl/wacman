#include "help_state.h"
#include "state_manager.h"
#include "config.h"

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

namespace pac
{
void pac::HelpState::on_enter()
{
    m_splash_texture = get_renderer().load_texture("res/splash_screen.png");

    /* Add input state that is blocking so no other input works */
    InputDomain pause_domain(true);
//    pause_domain.bind_key(GLFW_KEY_ESCAPE, ACTION_BACK);
    get_input().push(std::move(pause_domain));
}

void pac::HelpState::on_exit() { get_input().pop(); }

bool pac::HelpState::update(float dt)
{
    /* Begin Help Window, and center it */
    ImGui::SetNextWindowSize({600.f, 500.f});
    ImGui::SetNextWindowPos({SCREEN_W / 2.f, SCREEN_H / 2.f}, 0, {0.5f, 0.5f});
    ImGui::Begin("Helpwin", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

    /* Show Controls */
    ImGui::TextColored({1.f, 1.f, 0.f, 1.f}, "CONTROLS:");
    ImGui::Indent();
    ImGui::Columns(2);
    ImGui::Separator();
    ImGui::Text("WASD / Arrow Keys");
    ImGui::NextColumn();
    ImGui::Text("Move Pacman");
    ImGui::Separator();
    ImGui::NextColumn();
    ImGui::Text("ESC");
    ImGui::NextColumn();
    ImGui::Text("Pause / Go Back");
    ImGui::NextColumn();
    ImGui::Columns(1);
    ImGui::Unindent();

    ImGui::Separator();

    /* Show game rules */
    ImGui::TextColored({1.f, 1.f, 0.f, 1.f}, "\nRULES:");
    ImGui::Indent();
    ImGui::Text(
        " - To win you must eat all food items in the main level.\n"
        " - Eating ghost killers let you kill ghosts for the next 10 seconds\n"
        " - The three fruits in the middle give you many points,\n\tbut each one eaten will increase the speed of the ghosts\n"
        " - Fruits give more points for every ghost you have killed on\n\tyour current ghost killer.\n"
        " - You lose if you lose all of your lives.");
    ImGui::Unindent();

    /* Utility macro so I don't have to repeat this over and over */
#define SCORE_COL(lbl, score)       \
    do                              \
    {                               \
        ImGui::Separator();         \
        ImGui::Text("%s", (lbl));   \
        ImGui::NextColumn();        \
        ImGui::Text("%d", (score)); \
        ImGui::NextColumn();        \
    } while (0)

    /* Print score table */
    ImGui::TextColored({1.f, 1.f, 0.f, 1.f}, "\nSCORING:");
    ImGui::Indent();
    ImGui::Columns(2);
    SCORE_COL("Food", FOOD_SCORE);
    SCORE_COL("Ghost Killer", GHOST_KILLER_SCORE);
    SCORE_COL("Killing a Ghost", GHOST_KILL_SCORE);
    SCORE_COL("Fruit", POWERUP_SCORE);
    ImGui::Separator();
    ImGui::Columns(1);
    ImGui::TextColored({0.7f, 0.7f, 0.7f, 1.f},
                       "\nKilling a ghost is multiplied by number of ghosts killed.\nLikewise for fruit pickup.");
    ImGui::Unindent();

    /* Show extra credits */
    ImGui::TextColored({1.f, 1.f, 0.f, 1.f}, "\nEXTRA CREDITS:");
    ImGui::Indent();
    ImGui::TextColored({1.f, 1.f, 1.f, 0.9f}, "Andreas Findahl - Modified Pacman Theme Song");
    ImGui::Unindent();

    if (ImGui::Button("Back"))
    {
        m_context.state_manager->pop();
    }

    ImGui::End();

#undef SCORE_COL

    return false;
}

bool pac::HelpState::draw()
{
    get_renderer().draw({{SCREEN_W / 2.f, SCREEN_H / 2.f}, glm::vec2(SCREEN_W, SCREEN_H), {1.f, 1.f, 1.f}, m_splash_texture});
    return true;
}
}  // namespace pac
