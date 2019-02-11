#include "pacman.h"
#include <config.h>

namespace pac
{
/* Simply delegates work */
Pacman::Pacman() : Pacman({0, 0}) {}

pac::Pacman::Pacman(glm::ivec2 position) : m_position(position)
{
    m_textures.emplace(EDirection::North,
                       get_renderer().load_animation_texture("res/pacman.png", 7, 7 + 1 * 70, 70, 70, 4, 4));
    m_textures.emplace(EDirection::South,
                       get_renderer().load_animation_texture("res/pacman.png", 7, 7 + 0 * 70, 70, 70, 4, 4));
    m_textures.emplace(EDirection::West,
                       get_renderer().load_animation_texture("res/pacman.png", 7, 7 + 2 * 70, 70, 70, 4, 4));
    m_textures.emplace(EDirection::East,
                       get_renderer().load_animation_texture("res/pacman.png", 7, 7 + 3 * 70, 70, 70, 4, 4));
}

void Pacman::turn(EDirection new_direction) { m_desired_direction = new_direction; }

void Pacman::update(float dt)
{
    constexpr float frame_time = 1.f / 24.f;

    /* Update animation */
    m_animation_time += dt;
    while (m_animation_time > frame_time)
    {
        TextureID& current_texture = m_textures[m_current_direction];
        current_texture.frame_number = ++current_texture.frame_number % current_texture.frame_count;
        m_animation_time -= frame_time;
    }

    /* If we reach a new tile, update position */
    m_move_progress += m_speed * dt;
    if (m_move_progress >= 1.f)
    {
        glm::ivec2 to_position = {};
        switch (m_current_direction)
        {
        case EDirection::North: to_position = m_position + glm::ivec2{0, -1}; break;
        case EDirection::East: to_position = m_position + glm::ivec2{1, 0}; break;
        case EDirection::South: to_position = m_position + glm::ivec2{0, 1}; break;
        case EDirection::West: to_position = m_position + glm::ivec2{-1, 0}; break;
        }

        m_move_progress = 0.f;
        m_position = to_position;
    }
}

void Pacman::draw()
{
    glm::ivec2 to_position = {};
    switch (m_current_direction)
    {
    case EDirection::North: to_position = m_position + glm::ivec2{0, -1}; break;
    case EDirection::East: to_position = m_position + glm::ivec2{1, 0}; break;
    case EDirection::South: to_position = m_position + glm::ivec2{0, 1}; break;
    case EDirection::West: to_position = m_position + glm::ivec2{-1, 0}; break;
    }

    /* Interpolate position */
    glm::vec2 draw_position =
        (1.f - m_move_progress) * static_cast<glm::vec2>(m_position) + static_cast<glm::vec2>(to_position) * m_move_progress;

    /* Draw */
    get_renderer().draw({glm::vec2(TILE_SIZE<float> / 2.f, TILE_SIZE<float> / 2.f) + draw_position * TILE_SIZE<float>,
                         {TILE_SIZE<float>, TILE_SIZE<float>},
                         {1.f, 1.f, 1.f},
                         m_textures[m_current_direction]});
}
}  // namespace pac
