#include "ai_system.h"
#include "pathfinding.h"
#include "level.h"
#include "config.h"

namespace pac
{
extern entt::dispatcher g_event_queue;

AISystem::AISystem(entt::registry& reg, Level& level) : System(reg), m_level(level)
{
    g_event_queue.sink<EvEntityMoved>().connect<&AISystem::recieve>(this);
    g_event_queue.sink<EvPacInvulnreableChange>().connect<&AISystem::recieve_pacmanstate>(this);
}

AISystem::~AISystem() noexcept
{
    g_event_queue.sink<EvEntityMoved>().disconnect<&AISystem::recieve>(this);
    g_event_queue.sink<EvPacInvulnreableChange>().disconnect<&AISystem::recieve_pacmanstate>(this);
}

void AISystem::update(float dt)
{
    /* Update paths when required */
    m_reg.view<CAI>().each([this, dt](uint32_t e, CAI& ai) {
        /* Get position of AI */
        const auto& ai_pos = m_reg.get<CPosition>(e);
        const auto& plr_pos = get_player_pos();

        /* Count up current state timer */
        ai.state_timer += dt;

        /* Update AI State based on conditions and current state */
        switch (ai.state)
        {
        /* Chase for 20 seconds, then scatter */
        case EAIState::Chasing:
            if (ai.state_timer > GHOST_CHASE_TIME)
            {
                GFX_DEBUG("Ghost scattering after chasing");
                ai.state = EAIState::Scattering;
                ai.state_timer = 0.f;
            }
            break;
        /* Scatter for 10 seconds, then search for player */
        case EAIState::Scattering:
            if (ai.state_timer > 10.f)
            {
                GFX_DEBUG("Ghost searching after scattering");
                ai.state = EAIState::Searching;
                ai.state_timer = 0.f;
            }
        /* Search until you see player, then chase */
        case EAIState::Searching:
            if (m_level.los(ai_pos.position, plr_pos))
            {
                GFX_DEBUG("Ghost chasing after searching");
                ai.state = EAIState::Chasing;
                ai.state_timer = 0.f;
            }
            break;
        case EAIState::Dead:
            if (ai_pos.position == ai_pos.spawn)
            {
                GFX_DEBUG("Ghost is Respawning from the Dead.");
                m_reg.get<CAnimationSprite>(e).tint = glm::vec3{1.f, 1.f, 1.f};
                ai.state = EAIState::Searching;
                ai.state_timer = 0.f;
            }
            break;
        default: break;
        }
    });
}

void AISystem::recieve(const EvEntityMoved& move)
{
    /* We only care about AI entities */
    if (!m_reg.has<CAI>(move.entity))
    {
        return;
    }

    /* Create a new path and then ask movement system to move in the direction of the path */
    pathfind(m_reg.get<CPosition>(move.entity), move.direction, get_player_pos(), m_reg.get<CAI>(move.entity));
    const auto new_direction = m_reg.get<CAI>(move.entity).path->get();
    m_reg.get<CMovement>(move.entity).desired_direction = new_direction;
}

void AISystem::recieve_pacmanstate(const EvPacInvulnreableChange& pac)
{
    if (pac.is_invulnerable)
    {
        GFX_DEBUG("Ghosts becoming scared.")
        m_reg.view<CAI>().each(
            [](uint32_t e, CAI& ai) { ai.state = (ai.state == EAIState::Dead) ? ai.state : EAIState::Scared; });
    }
    else
    {
        GFX_DEBUG("Ghosts no longer scared -> Searching")
        m_reg.view<CAI>().each(
            [](uint32_t e, CAI& ai) { ai.state = (ai.state == EAIState::Dead) ? ai.state : EAIState::Searching; });
    }
}

glm::ivec2 AISystem::get_player_pos() const
{
    glm::ivec2 out_pos{};
    m_reg.view<CPlayer, CPosition>().each([&out_pos](const CPlayer& plr, const CPosition& pos) { out_pos = pos.position; });
    return out_pos;
}

void AISystem::pathfind(const CPosition& pos, const glm::ivec2& ai_dir, const glm::ivec2& plr_pos, CAI& ai)
{
    /* Based on state, we will have different targets for the AI */
    switch (ai.state)
    {
    /* Chasing goes directly to the player */
    case EAIState::Chasing: ai.target = plr_pos; break;
    /* ChasingAhead goes ahead of the player, anticipating their moves (TODO : Add this state) */
    case EAIState::ChasingAhead: ai.target = plr_pos; break;
    /* Scattering means going to random areas nearby */
    case EAIState::Scattering: ai.target = m_level.find_closest_intersection(pos.position, ai_dir); break;
    /* Searching doesn't know where the player is, and will wander to the next intersection */
    case EAIState::Searching: ai.target = m_level.find_closest_intersection(plr_pos, -ai_dir); break;
    /* Fleeing means -> Get away from Pacman ASAP! */
    case EAIState::Scared: ai.target = m_level.find_sensible_escape_point(pos.position, ai_dir, plr_pos); break;
    /* When Dead -> Go to Death Point */
    case EAIState::Dead: ai.target = pos.spawn; break;
    }

    /* Create path to the requested location */
    ai.path = std::make_unique<Path>(m_level, pos.position, ai.target);
}
}  // namespace pac
