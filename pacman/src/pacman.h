#pragma once

#include "common.h"
#include "rendering/renderer.h"

#include <unordered_map>

#include <glm/vec2.hpp>

namespace pac
{
/*!
 * \brief The Pacman class contains pacman's movement and rendering logic for everything that only modifies this class and does
 * not rely on outside data (IE: Pacman does not handle collisions, but whoever owns him does (the level).
 */
class Pacman
{
private:
    /* Animation Textures */
    std::unordered_map<glm::ivec2, TextureID, detail::custom_ivec2_hash> m_textures = {};

    /* Direction data as an enum */
    glm::ivec2 m_current_direction = {0, -1};
    glm::ivec2 m_desired_direction = {0, -1};

    /* Grid position of Pacman */
    glm::ivec2 m_position = {};

    /* Interpolation between current grid position and next */
    float m_move_progress = 0.f;

    /* Speed - Tiles per second */
    float m_speed = 3.f;

    /* Animation timer */
    float m_animation_time = 0.f;

public:
    Pacman();
    Pacman(glm::ivec2 position);

    friend class Level;

    void turn(glm::ivec2 new_direction);

    void update(float dt);

    void draw();

    /* Get the current/desired direction as a vector */
    glm::ivec2 current_direction() const;

    glm::ivec2 desired_direction() const;

private:
    /*!
     * \brief is_opposite checks if the given direction is the opposite of the given because pacman can not do a 180
     * \param dir is the direction to check
     * \return true if it's the opposite direction
     */
    bool is_opposite(glm::ivec2 dir);
};
}  // namespace pac
