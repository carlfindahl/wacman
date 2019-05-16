#pragma once

#include "system.h"

#include <glm/vec2.hpp>

namespace pac
{
class Level;

class AISystem : public System
{
private:
    Level& m_level;

public:
    AISystem(Level& level) : m_level(level) {}

    void update(float dt, entt::registry& reg) override;

private:
    /*!
     * \brief get_player_pos gets the position of the player
     * \param reg is the registry
     * \return player position
     */
    glm::ivec2 get_player_pos(entt::registry& reg) const;

    /*!
     * \brief get_player_ahead_pos gets some available tile in front of the player
     * \param reg is the registry
     * \return player position
     */
    glm::ivec2 get_player_ahead_pos(entt::registry& reg) const;
};
}  // namespace pac
