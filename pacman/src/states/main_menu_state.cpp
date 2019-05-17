#include "main_menu_state.h"
#include "game_state.h"
#include "high_score_state.h"
#include "audio/sound_manager.h"
#include "respawn_state.h"
#include "state_manager.h"
#include "editor_state.h"
#include "help_state.h"
#include "config.h"

#include <gfx.h>
#include <imgui/imgui.h>

namespace pac
{
void pac::MainMenuState::on_enter()
{
    m_splash_texture = get_renderer().load_texture("res/textures/splash_screen.png");
    get_sound().play("pacman");
    m_music_id = get_sound().play("simple_theme");
}

void pac::MainMenuState::on_exit() {}

bool pac::MainMenuState::update(float dt)
{
    /* Begin Main Menu Window, and center it */
    ImGui::SetNextWindowSize({160, 220});
    ImGui::SetNextWindowPos({SCREEN_W / 2.f, SCREEN_H / 2.f}, 0, {0.5f, 0.5f});
    ImGui::Begin("Main Menu", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

    if (ImGui::Button("Start Game", {150, 50}))
    {
        /* Stop music if still playing to avoid massive music load */
        get_sound().stop(m_music_id);
        m_context.state_manager->push<GameState>(m_context, "level0");
        m_context.state_manager->push<RespawnState>(m_context);
    }

    if (ImGui::Button("High Scores", {150, 50}))
    {
        m_context.state_manager->push<HighScoreState>(m_context);
    }

    if (ImGui::Button("Editor", {150, 50}))
    {
        m_context.state_manager->push<EditorState>(m_context);
    }

    if (ImGui::Button("Help / Credits", {150, 50}))
    {
        m_context.state_manager->push<HelpState>(m_context);
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
