#pragma once

#include "system.h"

namespace pac
{
class Level;

class GameSystem : public System
{
private:
public:
    void update(float dt, entt::registry& reg) override;
};
}  // namespace pac
