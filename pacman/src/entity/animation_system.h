#pragma once

#include "system.h"

namespace pac
{
/*!
 * \brief The AnimationSystem handles sprite animation updates
 */
class AnimationSystem : public System
{
public:
    using System::System;

    void update(float dt) override;
};
}  // namespace pac
