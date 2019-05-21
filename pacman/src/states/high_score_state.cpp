#include "high_score_state.h"
#include "state_manager.h"
#include "config.h"
#include "input/input.h"

#include <fstream>

#include <imgui/imgui.h>
#include <GLFW/glfw3.h>

namespace pac
{
void HighScoreState::on_enter()
{
    m_splash_texture = get_renderer().load_texture("res/textures/hiscore_screen.png");

    InputDomain hs_input_state(true);
    hs_input_state.bind_key(GLFW_KEY_ESCAPE, ACTION_BACK);
    get_input().push(std::move(hs_input_state));

    m_entries = load_high_score_entries_from_file();
}

void HighScoreState::on_exit() { get_input().pop(); }

bool HighScoreState::update(float dt)
{
    /* Position high score table properly */
    ImGui::SetNextWindowPos({SCREEN_W / 2.f, SCREEN_H / 2.f + 60.f}, 0, {.5f, .5f});
    ImGui::SetNextWindowSize({400.f, 500.f});
    ImGui::Begin("High Score", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

    if (ImGui::BeginTabBar("ScoreTabBar"))
    {
        ImGui::EndTabBar();
    }

    if (ImGui::Button("Back"))
    {
        m_context.state_manager->pop();
    }

    ImGui::End();
    return false;
}

bool HighScoreState::draw()
{
    get_renderer().draw({{SCREEN_W / 2.f, SCREEN_H / 2.f}, glm::vec2(SCREEN_W, SCREEN_H), {1.f, 1.f, 1.f}, m_splash_texture});
    return false;
}

void HighScoreState::scores_for(const std::string& level_name)
{
    /* Set up a 3 column high score table */
    ImGui::Columns(3, "ScoreColumns", false);
    ImGui::SetColumnWidth(0, 25.f);
    ImGui::SetColumnWidth(1, 275.f);

    /* Print score for each element */
    int i = 1;
    for (const auto& e : m_entries.at(level_name))
    {
        ImGui::Text("%d", i++);
        ImGui::NextColumn();
        ImGui::Text("%s", e.name.c_str());
        ImGui::NextColumn();
        ImGui::Text("%d", e.score);
        ImGui::NextColumn();
        ImGui::Separator();
    }
    ImGui::Columns();
}

}  // namespace pac
