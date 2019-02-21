#pragma once

#include "common.h"
#include "rendering/renderer.h"

#include <utility>
#include <unordered_map>

#include <glm/vec2.hpp>

namespace pac
{
class Path;

/*!
 * \brief The Pacman class contains pacman's movement and rendering logic for everything that only modifies this class and does
 * not rely on outside data (IE: Pacman does not handle collisions, but whoever owns him does (the level).
 */
class Ghost
{
public:
    /*!
     * \brief The EState enum describes the current state of the Ghost
     */
    enum class EState : uint16_t
    {
        Scattering,
        Chasing,
        Scared
    };

private:
    /* Animation Textures */
    std::unordered_map<glm::ivec2, TextureID, detail::custom_ivec2_hash> m_textures{};

    /* Grid position of Pacman */
    glm::ivec2 m_position = {};

    /* Ghost movement direction */
    glm::ivec2 m_direction = {0, 0};

    /* Home position */
    glm::ivec2 m_home = {};

    /* Interpolation between current grid position and next */
    float m_move_progress = 0.f;

    /* Speed - Tiles per second */
    float m_speed = 3.15f;

    /* Animation timer */
    float m_animation_time = 0.f;

    /* The Ghost AI State */
    EState m_ai_state = EState::Chasing;

    /* Path we are following */
    Path* m_path = nullptr;

    /* Are you a dead ghost */
    bool m_dead = false;

public:
    Ghost();

    explicit Ghost(glm::ivec2 position);

    Ghost(const Ghost& other);

    Ghost& operator=(const Ghost& other);

    Ghost(Ghost&& other) noexcept;

    Ghost& operator=(Ghost&& other) noexcept;

    ~Ghost();

    void update(float dt);

    void draw(bool blue = false);

    /*!
     * \brief ai_state get the ai state of the ghost
     * \return the current ai state
     */
    EState ai_state() const;

    /*!
     * \brief set_ai_state set the ai state of the ghost
     * \param ai_state the new ai state of the ghost
     */
    void set_ai_state(EState ai_state);

    /*!
     * \brief position get the position of the ghost
     * \return the ghost position
     */
    glm::ivec2 position() const;

    /*!
     * \brief set_position set the position of the ghost
     * \param pos is the new position
     */
    void set_position(glm::ivec2 pos);

    /*!
     * \brief direction get current direction of ghost
     * \return the direction the ghost is moving in
     */
    glm::ivec2 direction() const;

    /*!
     * \brief home get the home position of the Ghost
     * \return the home position
     */
    glm::ivec2 home() const;

    /*!
     * \brief set_path sets the path of the Ghost to follow
     * \param path_on_heap is the new heap-allocated path to follow
     */
    void set_path(Path* path_on_heap);

    /*!
     * \brief requires_path_update checks if the path needs an update
     * \return true if the path should be updated
     */
    bool requires_path_update() const;

    /*!
     * \brief die kills the ghost
     */
    void die();

    /*!
     * \brief dead checks if ghost is dead
     * \return true if ghost is dead
     */
    bool dead() const;

private:
    /*!
     * \brief is_opposite checks if the given direction is the opposite of the given because pacman can not do a 180
     * \param dir is the direction to check
     * \return true if it's the opposite direction
     */
    bool is_opposite(glm::ivec2 dir);
};
}  // namespace pac
