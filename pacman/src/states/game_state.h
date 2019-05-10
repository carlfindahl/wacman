#pragma once

#include <level.h>
#include <rendering/uniform_buffer_object.h>
#include <states/state.h>

#include <robinhood/robinhood.h>
#include <entt/entity/prototype.hpp>

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

    /* Prototypes of entities */
    robin_hood::unordered_map<std::string, entt::prototype> m_prototypes{};

public:
    GameState(GameContext owner);

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;

private:
    /*!
     * \brief create_prototypes creates entity prototypes
     */
    void create_prototypes();
};
}  // namespace pac
