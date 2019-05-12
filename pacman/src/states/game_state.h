#pragma once

#include <level.h>
#include <entity/events.h>
#include <entity/system.h>
#include <rendering/uniform_buffer_object.h>
#include <states/state.h>

#include <memory>

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

    /* Active Systems */
    std::vector<std::unique_ptr<System>> m_systems{};

public:
    GameState(GameContext owner);

    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;

    void recieve(const EvInput& input);

private:
    /*!
     * \brief create_prototypes creates entity prototypes
     */
    void create_prototypes();

    /*!
     * \brief add_systems needed by this state
     */
    void add_systems();

};
}  // namespace pac
