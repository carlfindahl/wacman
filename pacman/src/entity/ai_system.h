#pragma once

#include "system.h"

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
};
}  // namespace pac
