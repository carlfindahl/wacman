#include "pause_state.h"
#include "state_manager.h"
#include "main_menu_state.h"
#include "config.h"

#include <GLFW/glfw3.h>

namespace pac
{
void PauseState::on_enter()
{
    m_splash = get_renderer().load_texture("res/pause_screen.png");

    InputDomain pause_input(true);

    /* Resume game */
    pause_input.bind_key(GLFW_KEY_ESCAPE, [this] { m_context.state_manager->pop(); });
    pause_input.bind_key(GLFW_KEY_P, [this] { m_context.state_manager->pop(); });

    /* Quit to main menu */
    pause_input.bind_key(GLFW_KEY_Q, [this] {
        m_context.state_manager->clear();
        m_context.state_manager->push<MainMenuState>(m_context);
    });

    get_input().push(std::move(pause_input));
}

void PauseState::on_exit() { get_input().pop(); }

bool PauseState::update(float dt) { return false; }

bool PauseState::draw()
{
    get_renderer().draw({{SCREEN_W / 2.f, SCREEN_H / 2.f}, glm::vec2(SCREEN_W, SCREEN_H), {1.f, 1.f, 1.f}, m_splash});
    return false;
}

}  // namespace pac
