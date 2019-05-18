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
    AISystem(entt::registry& reg, Level& level) : System(reg), m_level(level) {}

    void update(float dt) override;

private:
    /*!
     * \brief get_player_pos gets the position of the player
     * \param reg is the registry
     * \return player position
     */
    glm::ivec2 get_player_pos() const;
};
}  // namespace pac
