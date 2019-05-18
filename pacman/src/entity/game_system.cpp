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

        /* Do similar check for ghosts */
        auto enemies = m_reg.group<CAI>(entt::get<const CPosition, CAnimationSprite>);
        for (auto ghost : enemies)
        {
            if (enemies.get<const CPosition>(ghost).position == pos.position)
            {
                if (plr.invulnerable > 0.f)
                {
                    /* If ghost is not dead, and not at their spawn then add score */
                    if (enemies.get<CAI>(ghost).state != EAIState::Dead &&
                        enemies.get<const CPosition>(ghost).position != enemies.get<const CPosition>(ghost).spawn)
                    {
                        plr.score += 250;
                    }

                    /* Mark ghost as dead and set it's ting to someting sensible */
                    enemies.get<CAI>(ghost).state = EAIState::Dead;
                    enemies.get<CAnimationSprite>(ghost).tint = glm::vec3{0.05f, 0.05f, 1.f};
                }
                else
                {
                    --plr.lives;
                    /* TODO:  Push state manager so we respawn */
                }
            }
        }
    });

    /* Manage ... */
}

}  // namespace pac
