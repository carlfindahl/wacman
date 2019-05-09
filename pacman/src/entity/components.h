#pragma once

#include "input.h"
#include "rendering/renderer.h"

#include <glm/vec2.hpp>
#include <robinhood/robinhood.h>
#include <entt/core/hashed_string.hpp>

namespace pac
{
/* Position Component */
struct CPosition
{
    /* Grid position */
    glm::ivec2 position{};
};

/* Movement component */
struct CMovement
{
    /* Current movement direction */
    glm::ivec2 current_direction{};

    /* Desired movement direction */
    glm::ivec2 desired_direction{};

    /* Movement speed in tiles per second */
    float speed = 1.f;
};

/* Animated sprite component */
struct CAnimationSprite
{
    /* Available animations (accessible by hash of their name */
    robin_hood::unordered_map<entt::hashed_string, TextureID> available_animations{};

    /* The active animation sprite (from the map) */
    TextureID active_animation{};

    /* Current animation time */
    float animation_timer = 0.f;

    /* Number of FPS for animation */
    float fps = 24.f;
};

struct CSprite
{
    /* Sprite ID */
    TextureID sprite{};
};

/* Player component (Pacman) */
struct CPlayer
{
    /* Number of lives */
    int32_t lives = 3;

    /* Current score */
    int32_t score = 0;
};

/* Input Component */
struct CInput
{
    input::InputState input_state{false};
};

}  // namespace pac
