#pragma once

#include "state.h"
#include <rendering/renderer.h>

namespace pac
{
class MainMenuState : public State
{
private:
    /* Splash screen texture */
    TextureID m_splash_texture{};    

public:
    MainMenuState() = default;

    explicit MainMenuState(StateManager& owner) : State(owner) { m_splash_texture = get_renderer().load_texture("res/splash_screen.png"); }

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;
};

}  // namespace pac
