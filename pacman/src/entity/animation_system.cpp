#include "animation_system.h"
#include "components.h"

namespace pac
{
void pac::AnimationSystem::update(float dt, entt::registry& reg)
{
    /* Update animation frame */
    reg.view<CAnimationSprite>().each([dt](auto e, CAnimationSprite& spr) {
        spr.animation_timer += dt;

        /* If we hit a new frame, then update frame number */
        if (spr.animation_timer > 1.f / spr.fps)
        {
            spr.animation_timer = 0.f;
            spr.active_animation.frame_number = (spr.active_animation.frame_number + 1) % spr.active_animation.frame_count;
        }
    });
}

}  // namespace pac
