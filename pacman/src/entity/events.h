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

struct EvEntityMoved
{
    uint32_t entity{};
    glm::ivec2 direction{};
    glm::ivec2 new_position{};
};

struct EvPacInvulnreableChange
{
    bool is_invulnerable = false;
};

}  // namespace pac
