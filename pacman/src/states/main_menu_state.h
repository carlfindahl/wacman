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

    /* Music id */
    unsigned m_music_id = 0u;

public:
    using State::State;

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;
};

}  // namespace pac
