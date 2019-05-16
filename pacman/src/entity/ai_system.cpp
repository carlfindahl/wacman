#include "ai_system.h"
#include "components.h"
#include "pathfinding.h"

namespace pac
{
void AISystem::update(float dt, entt::registry& reg)
{
    /* Update paths when required */
    reg.view<CAI>().each([this, &reg](uint32_t e, CAI& ai) {
        /* Create a new path when needed */
        if (!ai.path || ai.path->outdated())
        {
            /* Based on state, we will have different targets for the AI */
            switch (ai.state)
            {
            /* Chasing goes directly to the player */
            case EAIState::Chasing: ai.target = get_player_pos(reg); break;
            /* ChasingAhead goes ahead of the player, anticipating their moves */
            case EAIState::ChasingAhead: ai.target = get_player_ahead_pos(reg); break;
            /* Searching doesn't know where the player is, and will wander to the next intersection */
            case EAIState::Searching: break;
            /* Scattering means going to my spawn */
            case EAIState::Scattering: break;
            /* Fleeing means -> Get away from Pacman ASAP! */
            case EAIState::Fleeing: break;
            }

            /* Pathfind to the requested location */
            ai.path = std::make_unique<Path>(m_level, reg.get<CPosition>(e).position, ai.target);
        }
    });
}

glm::ivec2 AISystem::get_player_pos(entt::registry& reg) const
{
    glm::ivec2 out_pos{};
    reg.view<CPlayer, CPosition>().each([&out_pos](const CPlayer& plr, const CPosition& pos) { out_pos = pos.position; });
    return out_pos;
}

glm::ivec2 AISystem::get_player_ahead_pos(entt::registry& reg) const
{
    glm::ivec2 out_pos{};
    reg.view<CPlayer, CPosition, CMovement>().each(
        [&out_pos](const CPlayer& plr, const CPosition& pos, const CMovement& mov) { out_pos = pos.position; });
    return out_pos;
}
}  // namespace pac
