#include "pause_state.h"
#include "config.h"
#include <rendering/renderer.h>

namespace pac
{
void PauseState::on_enter() {}

void PauseState::on_exit() {}

bool PauseState::update(float dt) { return false; }

bool PauseState::draw()
{
    get_renderer().draw({{SCREEN_W / 2.f, SCREEN_H / 2.f}, glm::vec2(SCREEN_W, SCREEN_H), {0.f, 0.f, 0.2f}, m_splash});
    return true;
}

}  // namespace pac
