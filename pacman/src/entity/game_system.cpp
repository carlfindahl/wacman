#include "game_system.h"
#include "entity/components.h"

namespace pac
{
void GameSystem::update(float dt)
{
    /* Add score to player when touching pickups */
    m_reg.view<CPlayer, const CPosition>().each([this](uint32_t e, CPlayer& plr, const CPosition& pos) {
        auto pickups = m_reg.group<CPickup>(entt::get<CPosition>);
        for (auto p : pickups)
        {
            if (pickups.get<CPosition>(p).position == pos.position)
            {
                plr.score += pickups.get<CPickup>(p).score;
                m_reg.destroy(p);
            }
        }
    });

    /* Manage ... */
}

}  // namespace pac
