#include "game_over_state.h"
#include "main_menu_state.h"
#include "state_manager.h"
#include "config.h"

#include <vector>
#include <cstring>
#include <algorithm>

#include <cglutil.h>
#include <imgui/imgui.h>

namespace pac
{
void GameOverState::on_enter()
{
    m_playername.fill('\0');

    /* Add input state that is blocking so no other input works */
    InputDomain pause_input(true);
    get_input().push(std::move(pause_input));
}

void GameOverState::on_exit()
{
    /* Load and sort entries, then write back to file */
    std::vector<ScoreEntry> entries = load_high_score_entries_from_file();

    if (strlen(m_playername.data()) > 0)
    {
        entries.push_back({m_playername.data(), m_score});
        std::sort(entries.begin(), entries.end(), [](const ScoreEntry& a, const ScoreEntry& b) { return a.score > b.score; });
        write_high_score_entries_to_file(entries);
    }

    get_input().pop();
}

bool GameOverState::update(float dt)
{
    ImGui::SetNextWindowPos({SCREEN_W / 2.f, SCREEN_H / 2.f + 60.f}, 0, {.5f, .5f});
    ImGui::SetNextWindowSize({375.f, 80.f});
    ImGui::Begin("New Hi-Score", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

    /* Print title centered */
    ImGui::Columns(3, nullptr, false);
    ImGui::NextColumn();
    ImGui::Text("%s", m_title);
    ImGui::NextColumn();
    ImGui::NextColumn();
    ImGui::Columns(1);

    /* Print submission form */
    ImGui::Text("Name");
    ImGui::SameLine();
    ImGui::InputText("", m_playername.data(), m_playername.size());
    ImGui::SameLine();
    if (ImGui::Button("Submit"))
    {
        m_context.state_manager->clear();
        m_context.state_manager->push<MainMenuState>(m_context);
    }

    /* Score text */
    ImGui::TextColored({1.f, 1.f, 0.f, 1.f}, "You scored %d points.", m_score);

    ImGui::End();
    return false;
}

bool GameOverState::draw() { return true; }

}  // namespace pac
