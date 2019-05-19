#pragma once

#include "system.h"
#include "common.h"
#include "events.h"

namespace pac
{
class Level;

class GameSystem : public System
{
private:
    /* Context */
    GameContext m_context{};

public:
    GameSystem(entt::registry& reg, GameContext context);

    ~GameSystem() noexcept override;

    void update(float dt) override;

    void recieve(const EvPacLifeChanged& life_update);
};
}  // namespace pac
