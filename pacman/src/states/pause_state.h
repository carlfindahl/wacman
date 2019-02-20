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
    using State::State;

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;
};

}  // namespace pac
