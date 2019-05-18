#pragma once

#include "system.h"

namespace pac
{
class Level;

class MovementSystem : public System
{
private:
    Level& m_level;

public:
    MovementSystem(entt::registry& reg, Level& level) : System(reg), m_level(level) {}

    void update(float dt) override;
};
}  // namespace pac
