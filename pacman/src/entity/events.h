#pragma once

#include "input/input.h"

namespace pac
{
struct EvInput
{
    Action action = ACTION_NONE;
};

struct EvMouseMove
{
    glm::vec2 delta{};
    glm::vec2 position{};
};

}  // namespace pac
