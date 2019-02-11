#pragma once

#include "rendering/renderer.h"

#include <unordered_map>

#include <glm/vec2.hpp>

namespace pac
{
enum class EDirection
{
    North,
    South,
    East,
    West
};

class Pacman
{
private:
    /* Animation Textures */
    std::unordered_map<EDirection, TextureID> m_textures = {};

    /* Direction data */
    EDirection m_current_direction = EDirection::East;
    EDirection m_desired_direction = EDirection::East;

    /* Grid position of Pacman */
    glm::ivec2 m_position = {};

    /* Interpolation between current grid position and next */
    float m_move_progress = 0.f;

    /* Speed - Tiles per second */
    float m_speed = 2.f;

    /* Animation timer */
    float m_animation_time = 0.f;

public:
    Pacman();
    Pacman(glm::ivec2 position);

    void turn(EDirection new_direction);

    void update(float dt);

    void draw();
};
}  // namespace pac
