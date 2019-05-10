#pragma once

#include "system.h"

namespace pac
{
/*!
 * \brief The RenderingSystem class takes care of drawing and interpolating entities with sprite components
 */
class RenderingSystem : public System
{
private:

public:
    void update(float dt, entt::registry& reg) override;
};
}  // namespace pac
