#pragma once

#include "system.h"

namespace pac
{
class AnimationSystem : public System
{
private:
public:
    void update(float dt, entt::registry& reg) override;
};
}  // namespace pac
