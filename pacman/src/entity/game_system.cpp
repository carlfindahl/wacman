#include "game_system.h"
#include "states/state_manager.h"
#include "states/respawn_state.h"
#include "states/game_over_state.h"
#include "entity/components.h"
#include "config.h"

namespace pac
{
extern entt::dispatcher g_event_queue;

GameSystem::GameSystem(entt::registry& reg, GameContext context) : System(reg), m_context(context)
{
    g_event_queue.sink<EvPacLifeChanged>().connect<&GameSystem::recieve>(*this);
}

GameSystem::~GameSystem() noexcept { g_event_queue.sink<EvPacLifeChanged>().disconnect<&GameSystem::recieve>(*this); }

void GameSystem::update(float dt)
{
    /* Do player only updates */
    m_reg.view<CPlayer>().each([dt, this](CPlayer& plr) {
        /* Turn of invulnerable when timer too low */
        if (plr.invulnerable > 0.f && plr.invulnerable - dt <= 0.f)
        {
            /* When vulnerable, go back to a multiplier of 0 */
            plr.ghosts_killed = 0;
            g_event_queue.trigger<EvPacInvulnreableChange>(false);
        }
        plr.invulnerable -= dt;

        /* Check if the game is won */
        if (m_reg.view<CPickup>().empty())
        {
            g_event_queue.enqueue(EvLevelFinished{true, plr.score});
        }
    });

    /* Add score to player when touching pickups */
    m_reg.view<CPlayer, CPosition>().each([this](entt::entity e, CPlayer& plr, CPosition& pos) {
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
                        /* Multiply by number of ghosts killed */
                        ++plr.ghosts_killed;
                        plr.score += GHOST_KILL_SCORE * plr.ghosts_killed;
                    }

                    /* Mark ghost as dead and set it's ting to someting sensible */
                    enemies.get<CAI>(ghost).state = EAIState::Dead;
                    enemies.get<CAnimationSprite>(ghost).tint = glm::vec3{0.05f, 0.05f, 1.f};
                }
                else
                {
                    /* Subtract player life and publish life changed event */
                    --plr.lives;
                    g_event_queue.enqueue(EvPacLifeChanged{e, -1, plr.lives});
                }
            }
        }
    });
}

void GameSystem::recieve(const EvPacLifeChanged& life_update)
{
    /* GAME OVER */
    if (life_update.new_life <= 0)
    {
        g_event_queue.enqueue(EvLevelFinished{false, m_reg.get<CPlayer>(life_update.pacman).score});
    }
    /* Lost a life = Move everything to their spawn point and push respawn context */
    else if (life_update.delta < 0)
    {
        m_context.state_manager->push<RespawnState>(m_context, 1.75f, "Respawning...");
        m_reg.view<CPosition>().each([](CPosition& pos) { pos.position = pos.spawn; });
        m_reg.view<CMovement>().each([](CMovement& mov) { mov.current_direction = {0, 0}; });
        m_reg.view<CAI>().each([](CAI& ai) {
            ai.state = EAIState::Searching;
            ai.state_timer = 0.f;
        });
    }
    /* Gained a life */
    else if (life_update.delta > 0)
    {
    }
}

}  // namespace pac
