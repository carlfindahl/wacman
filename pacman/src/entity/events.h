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

struct EvPacLifeChanged
{
    uint32_t pacman{};
    int delta = 0;
    int new_life = 0;
};

struct EvLevelFinished
{
    /* Did you win or not */
    bool won = false;

    /* Your final score */
    int final_score = 0;
};

}  // namespace pac
