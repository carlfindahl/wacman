#pragma once

#include "system.h"
#include "components.h"

namespace pac
{
class Level;

/*!
 * \brief The MovementSystem takes care of handling movement updates for all entities that desire to move
 */
class MovementSystem : public System
{
private:
    Level& m_level;

public:
    MovementSystem(entt::registry& reg, Level& level) : System(reg), m_level(level) {}

    void update(float dt) override;

private:
    void update_animation(glm::ivec2 new_direction, CAnimationSprite& anim);
};
}  // namespace pac
