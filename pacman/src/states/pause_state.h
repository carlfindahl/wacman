#pragma once

#include "state.h"
#include <rendering/renderer.h>

namespace pac
{
class PauseState : public State
{
private:
    /* Pause Screen Splash */
    TextureID m_splash{};

public:
    PauseState() = default;

    explicit PauseState(StateManager& owner) : State(owner)
    { 
        m_splash = get_renderer().load_texture("res/pause_screen.png");       
    }

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;
};

}  // namespace pac
