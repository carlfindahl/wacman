#pragma once

#include <level.h>
#include <rendering/uniform_buffer_object.h>
#include <states/state.h>

namespace pac
{
class GameState : public State
{
private:
    /* The level / world */
    Level m_level = {};

public:
    GameState();

    ~GameState() noexcept override;

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;
};
}  // namespace pac
