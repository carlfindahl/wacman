#pragma once

#include "system.h"
#include "events.h"

namespace pac
{
/*!
 * \brief The RenderingSystem class takes care of drawing and interpolating entities with sprite components
 */
class InputSystem : public System
{
private:

public:
    void update(float dt, entt::registry& reg) override;

    void recieve(const EvInput& input);
};
}  // namespace pac
