#pragma once

#include "state.h"
#include "rendering/renderer.h"

namespace pac
{
class MainMenuState : public State
{
private:
    /* Splash screen texture */
    TextureID m_splash_texture{};

public:
    using State::State;

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;
};

}  // namespace pac
