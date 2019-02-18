#include "pause_state.h"
#include "state_manager.h"
#include "config.h"

#include <GLFW/glfw3.h>

namespace pac
{
PauseState::PauseState(StateManager& owner) : State(owner) { m_splash = get_renderer().load_texture("res/pause_screen.png"); }

void PauseState::on_enter()
{
    input::InputState pause_input(true);
    pause_input.set_binding(GLFW_KEY_ESCAPE, [this] { m_owner->pop(); });
    pause_input.set_binding(GLFW_KEY_P, [this] { m_owner->pop(); });

    auto& input_manager = input::get_input();
    input_manager.push(std::move(pause_input));
}

void PauseState::on_exit() { input::get_input().pop(); }

bool PauseState::update(float dt) { return false; }

bool PauseState::draw()
{
    get_renderer().draw({{SCREEN_W / 2.f, SCREEN_H / 2.f}, glm::vec2(SCREEN_W, SCREEN_H), {1.f, 1.f, 1.f}, m_splash});
    return true;
}

}  // namespace pac
