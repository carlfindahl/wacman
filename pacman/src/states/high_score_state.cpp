#include "high_score_state.h"
#include "state_manager.h"
#include "config.h"
#include "input.h"

#include <GLFW/glfw3.h>

namespace pac
{
HighScoreState::HighScoreState(StateManager& owner) : State(owner)
{
    m_splash_texture = get_renderer().load_texture("res/hiscore_screen.png");
}

void HighScoreState::on_enter()
{
    input::InputState hs_input_state(true);
    hs_input_state.set_binding(GLFW_KEY_ESCAPE, [this] { m_owner->pop(); });
    input::get_input().push(std::move(hs_input_state));
}

void HighScoreState::on_exit() { input::get_input().pop(); }

bool HighScoreState::update(float dt) { return false; }

bool HighScoreState::draw()
{
    get_renderer().draw({{SCREEN_W / 2.f, SCREEN_H / 2.f}, glm::vec2(SCREEN_W, SCREEN_H), {1.f, 1.f, 1.f}, m_splash_texture});
    return false;
}

}  // namespace pac
