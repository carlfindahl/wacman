#include "respawn_state.h"
#include "state_manager.h"
#include "config.h"

#include <imgui/imgui.h>
#include <GLFW/glfw3.h>

namespace pac
{
RespawnState::RespawnState(GameContext context, float wait_seconds, std::string_view prefix_msg)
    : State(context), m_respawn_timer(wait_seconds), m_prefix_msg(prefix_msg)
{
}

void RespawnState::on_enter()
{
    /* Add input state that is blocking so no other input works */
    InputState pause_input(true);
    get_input().push(std::move(pause_input));
}

void RespawnState::on_exit() { get_input().pop(); }

bool RespawnState::update(float dt)
{
    /* When timer goes to zero, then remove the state */
    m_respawn_timer -= std::chrono::duration<float>(dt);
    if (m_respawn_timer < std::chrono::duration<float>(0.f))
    {
        m_context.state_manager->pop();
    }

    /* Show respawn text */
    ImGui::SetNextWindowSize({270.f, 16.f});
    ImGui::SetNextWindowPos({SCREEN_W / 2.f, SCREEN_H / 2.f + 60.f}, 0, {.5f, .5f});
    ImGui::Begin("RespawnWindow", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
    ImGui::Text("%s IN %3.1f SECONDS!", m_prefix_msg.c_str(), m_respawn_timer.count());
    ImGui::End();
    return false;
}

bool RespawnState::draw() { return true; }

}  // namespace pac
