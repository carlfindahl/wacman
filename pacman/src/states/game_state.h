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
    Level m_level;

    /* Game overlay */
    TextureID m_overlay{};

    /* ID of music track */
    unsigned m_music_id = 0u;

public:
    GameState(GameContext owner);

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;
};
}  // namespace pac
