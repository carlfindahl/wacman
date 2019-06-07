#include "movement_system.h"
#include "components.h"
#include "events.h"
#include "level.h"

#include <gfx.h>

namespace pac
{
extern entt::dispatcher g_event_queue;

void MovementSystem::update(float dt)
{
    auto movement_group = m_reg.group<CPosition, CMovement>(entt::get<CCollision>);
    movement_group.each([dt, this](uint32_t e, CPosition& pos, CMovement& mov, const CCollision& _) {
        /* Check if we can move towards desired direction and switch it if possible */
        if (mov.desired_direction != mov.current_direction && !m_level.will_collide(pos.position, mov.desired_direction) &&
            mov.progress < 0.35f)
        {
            /* 180 degree turns revert movement progress */
            if (glm::distance(glm::vec2(mov.current_direction), glm::vec2(mov.desired_direction)) == 2)
            {
                mov.progress = 0.f;
            }
            /* If we are a player in a 90 degree turn then we get a boost (like original game) */
            else if (m_reg.has<CPlayer>(e))
            {
                mov.progress += 0.25f;
            }

            /* Set direction */
            mov.current_direction = mov.desired_direction;

            /* Auto-update direction animation if mover has anim sprite */
            if (auto* anim = m_reg.try_get<CAnimationSprite>(e); anim)
            {
                update_animation(mov.current_direction, *anim);
            }
        }

        /* If we will collide in current direction then just skip any other logic */
        if (m_level.will_collide(pos.position, mov.current_direction))
        {
            mov.progress = 0.f;
            return;
        }

        /* Update the progress based on speed */
        mov.progress += dt * mov.speed;

        /* When we reach a new tile, then reset progress and update position (required component) */
        if (mov.progress >= 1.f)
        {
            mov.progress = 0.f;
            pos.position += mov.current_direction;

            /* If there is a teleporter at the new destination, teleport */
            if (auto dest = m_level.get_teleport_dest(pos.position); dest)
            {
                pos.position = dest.value().position;
                mov.current_direction = dest.value().direction;
            }

            /* Publish event that entity has moved */
            g_event_queue.enqueue(EvEntityMoved{e, mov.current_direction, pos.position});
        }
    });
}

void MovementSystem::update_animation(glm::ivec2 new_direction, CAnimationSprite& anim)
{
    /* Update animation direction automatically */
    if (new_direction == glm::ivec2{1, 0})
    {
        anim.active_animation = anim.available_animations["right"];
    }
    else if (new_direction == glm::ivec2{-1, 0})
    {
        anim.active_animation = anim.available_animations["left"];
    }
    else if (new_direction == glm::ivec2{0, -1})
    {
        anim.active_animation = anim.available_animations["up"];
    }
    else
    {
        anim.active_animation = anim.available_animations["down"];
    }
}

}  // namespace pac
