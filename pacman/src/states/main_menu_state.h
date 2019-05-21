#pragma once

#include "state.h"
#include "rendering/renderer.h"
#include "ui.h"

namespace pac
{
class MainMenuState : public State
{
private:
    /* Splash screen texture */
    TextureID m_splash_texture{};

    /* Level Select UI */
    ui::LevelSelector m_selector_ui{m_context};

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
