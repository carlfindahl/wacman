#include "game_system.h"
#include "entity/components.h"
#include "events.h"
#include "config.h"

namespace pac
{
extern entt::dispatcher g_event_queue;

void GameSystem::update(float dt)
{
    /* Do player only updates */
    m_reg.view<CPlayer>().each([dt](CPlayer& plr) {
        /* Turn of invulnerable when timer too low */
        if (plr.invulnerable > 0.f && plr.invulnerable - dt <= 0.f)
        {
            g_event_queue.trigger<EvPacInvulnreableChange>(false);
        }
        plr.invulnerable -= dt;
    });

    /* Add score to player when touching pickups */
    m_reg.view<CPlayer, const CPosition>().each([this](uint32_t e, CPlayer& plr, const CPosition& pos) {
        auto pickups = m_reg.group<CPickup>(entt::get<CPosition>);
        for (auto p : pickups)
        {
            if (pickups.get<CPosition>(p).position == pos.position)
            {
                /* If we picked up a "ghostkiller" then set invulnerable */
                if (auto* meta = m_reg.try_get<CMeta>(p); meta && meta->name == "ghostkiller")
                {
                    plr.invulnerable = GHOST_KILLER_TIME;
                    g_event_queue.trigger<EvPacInvulnreableChange>(true);
                }

                /* Add pickup score to player */
                plr.score += pickups.get<CPickup>(p).score;
                m_reg.destroy(p);
            }
        }
    });

    /* Manage ... */
}

}  // namespace pac
