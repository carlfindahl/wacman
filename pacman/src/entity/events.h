#pragma once

#include "input/input.h"

namespace pac
{
struct InputEvent
{
    Action action = ACTION_NONE;
};

struct EvMouseMove
{
    glm::vec2 delta{};
    glm::vec2 position{};
    Action action = ACTION_NONE;
};

}  // namespace pac
