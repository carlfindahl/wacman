#include "movement_system.h"
#include "components.h"

namespace pac
{
void MovementSystem::update(float dt, entt::registry& reg)
{
    auto movement_view = reg.view<CMovement>();
    for (auto e : movement_view)
    {
        /* Update the progress based on speed */
        auto mc = movement_view.get(e);
        mc.progress += dt * mc.speed;

        /* When we reach a new tile, then reset progress and update position (required component) */
        if (mc.progress >= 1.f)
        {
            mc.progress = 0.f;
            reg.get<CPosition>(e).position += mc.current_direction;
        }
    }
}

}  // namespace pac
