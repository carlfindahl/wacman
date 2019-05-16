#include "ai_system.h"
#include "components.h"
#include "pathfinding.h"
#include "level.h"

namespace pac
{
void AISystem::update(float dt, entt::registry& reg)
{
    /* Update paths when required */
    reg.view<CAI>().each([this, &reg, dt](uint32_t e, CAI& ai) {
        /* Get position of AI */
        const auto& ai_pos = reg.get<CPosition>(e).position;
        const auto& plr_pos = get_player_pos(reg);

        /* Create a new path when needed */
        if (!ai.path || ai.path->outdated())
        {
            /* Based on state, we will have different targets for the AI */
            switch (ai.state)
            {
            /* Chasing goes directly to the player */
            case EAIState::Chasing: ai.target = plr_pos; break;
            /* ChasingAhead goes ahead of the player, anticipating their moves (TODO) */
            case EAIState::ChasingAhead: ai.target = plr_pos; break;
            /* Searching doesn't know where the player is, and will wander to the next intersection */
            case EAIState::Searching: ai.target = m_level.find_closest_intersection(ai_pos); break;
            /* Scattering means going to random locations */
            case EAIState::Scattering: break;
            /* Fleeing means -> Get away from Pacman ASAP! */
            case EAIState::Scared: break;
            /* When Dead -> Go to Death Point */
            case EAIState::Dead: break;
            }

            /* Create path to the requested location */
            ai.path = std::make_unique<Path>(m_level, ai_pos, ai.target);
        }

        /* Count up current state timer */
        ai.state_timer += dt;

        /* Update AI State based on conditions and current state */
        switch (ai.state)
        {
        /* Chase for 20 seconds, then scatter */
        case EAIState::Chasing:
            if (ai.state_timer > 20.f)
            {
                ai.state = EAIState::Scattering;
                ai.state_timer = 0.f;
            }
            break;
        /* Scatter for 10 seconds, then search for player */
        case EAIState::Scattering:
            if (ai.state_timer > 10.f)
            {
                ai.state = EAIState::Searching;
                ai.state_timer = 0.f;
            }
        /* Search until you see player, then chase */
        case EAIState::Searching:
            if (m_level.los(ai_pos, plr_pos))
            {
                ai.state = EAIState::Chasing;
                ai.state_timer = 0.f;
            }
            break;
        case EAIState::Scared: /* Exit when player is not invulnerable */ break;
        case EAIState::Dead: /* Exit when reached home */ break;
        default: break;
        }
    });
}

glm::ivec2 AISystem::get_player_pos(entt::registry& reg) const
{
    glm::ivec2 out_pos{};
    reg.view<CPlayer, CPosition>().each([&out_pos](const CPlayer& plr, const CPosition& pos) { out_pos = pos.position; });
    return out_pos;
}
}  // namespace pac
