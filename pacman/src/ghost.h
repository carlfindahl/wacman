#pragma once

#include "rendering/renderer.h"

#include <utility>
#include <unordered_map>

#include <glm/vec2.hpp>

namespace pac
{
namespace detail
{
/*!
 * \brief The custom_ivec2_hash struct is a quick hash that puts x in upper 32 bits and y in lower. Used for mapping dirs to textures
 */
struct custom_ivec2_hash
{
    std::size_t operator()(const glm::ivec2& vec) const noexcept
    {
        const uint64_t hash = (static_cast<uint64_t>(vec.x) << 32u) | vec.y;
        return hash;
    }
};
}  // namespace detail
/*!
 * \brief The Pacman class contains pacman's movement and rendering logic for everything that only modifies this class and does
 * not rely on outside data (IE: Pacman does not handle collisions, but whoever owns him does (the level).
 */
class Ghost
{
private:
    /* Animation Textures */
    std::unordered_map<glm::ivec2, TextureID, detail::custom_ivec2_hash> m_textures{};

    /* Grid position of Pacman */
    glm::ivec2 m_position = {};

    /* Ghost movement direction */
    glm::ivec2 m_direction = {1, 0};

    /* Interpolation between current grid position and next */
    float m_move_progress = 0.f;

    /* Speed - Tiles per second */
    float m_speed = 4.15f;

    /* Animation timer */
    float m_animation_time = 0.f;

public:
    Ghost();

    explicit Ghost(glm::ivec2 position);

    void update(float dt);

    void draw();

private:
    /*!
     * \brief is_opposite checks if the given direction is the opposite of the given because pacman can not do a 180
     * \param dir is the direction to check
     * \return true if it's the opposite direction
     */
    bool is_opposite(glm::ivec2 dir);
};
}  // namespace pac
