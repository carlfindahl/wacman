#pragma once

#include "common.h"
#include "input/input.h"

#include <entt/entity/registry.hpp>

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
    entt::entity entity{};
    glm::ivec2 direction{};
    glm::ivec2 new_position{};
};

struct EvPacInvulnreableChange
{
    bool is_invulnerable = false;
};

struct EvPacPickup
{
    std::string pickup_name{};
    int score_delta;
};

struct EvGhostStateChanged
{
    entt::entity ghost{};
    EAIState new_state{};
};

struct EvPacLifeChanged
{
    entt::entity pacman{};
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
