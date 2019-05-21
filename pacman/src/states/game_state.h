#pragma once

#include <entity/factory.h>

#include <level.h>
#include <entity/events.h>
#include <entity/system.h>
#include <rendering/uniform_buffer_object.h>
#include <states/state.h>

#include <memory>
#include <string_view>

#include <robinhood/robinhood.h>
#include <entt/entity/prototype.hpp>

namespace pac
{
class GameState : public State
{
private:
    /* The level / world */
    Level m_level{};

    /* Active Systems */
    std::vector<std::unique_ptr<System>> m_systems{};

    /* Game overlay */
    TextureID m_overlay{};

    /* ID of music track */
    unsigned m_music_id = 0u;

public:
    using State::State;

    GameState(GameContext owner, std::string_view level_name);

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;

    void recieve(const EvInput& input);

    void on_win_or_lose(const EvLevelFinished& data);

private:
    /*!
     * \brief add_systems needed by this state
     */
    void add_systems();
};
}  // namespace pac
