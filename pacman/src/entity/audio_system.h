#pragma once

#include "system.h"
#include <entt/signal/sigh.hpp>

namespace pac
{
class AudioSystem : public System
{
private:
    /* Scoped connections to events (so they autmatically unsubscribe on scope exit */
    entt::scoped_connection m_pickup_conn{};
    entt::scoped_connection m_ghost_conn{};
    entt::scoped_connection m_pacman_conn{};

public:
    AudioSystem(entt::registry& reg);

    void update(float dt) override;

    /**
     * @brief play pickup sound
     */
    void recieve_pickup(const struct EvPacPickup& pickup) const;

    /**
     * @brief play ghost death sound
     */
    void recieve_ghost_state(const struct EvGhostStateChanged& state) const;

    /**
     * @brief play pacman game over sound
     */
    void recieve_pacman_life(const struct EvPacLifeChanged& life) const;
};
}  // namespace pac
