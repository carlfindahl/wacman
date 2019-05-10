#pragma once

#include "system.h"

namespace pac
{
class MovementSystem : public System
{
private:
public:
    void update(float dt, entt::registry& reg) override;
};
}  // namespace pac
