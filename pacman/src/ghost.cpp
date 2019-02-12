#include "ghost.h"
#include <config.h>

namespace pac
{
Ghost::Ghost() : Ghost({0, 0}) {}

Ghost::Ghost(glm::ivec2 position) : m_position(position)
{
    /* Load Ghost textures */
    m_textures.emplace(glm::ivec2{0, -1}, get_renderer().load_animation_texture("res/pacman.png", 296, 7 + 1 * 70, 70, 70, 2, 2));
    m_textures.emplace(glm::ivec2{0, 1}, get_renderer().load_animation_texture("res/pacman.png", 296, 7 + 0 * 70, 70, 70, 2, 2));
    m_textures.emplace(glm::ivec2{-1, 0}, get_renderer().load_animation_texture("res/pacman.png", 296, 7 + 2 * 70, 70, 70, 2, 2));
    m_textures.emplace(glm::ivec2{1, 0}, get_renderer().load_animation_texture("res/pacman.png", 296, 7 + 3 * 70, 70, 70, 2, 2));
}

void Ghost::update(float dt)
{
    constexpr float frame_time = 1.f / 24.f;

    /* Update animation */
    m_animation_time += dt;
    while (m_animation_time > frame_time)
    {
        TextureID& current_texture = m_textures[m_direction];
        current_texture.frame_number = ++current_texture.frame_number % current_texture.frame_count;
        m_animation_time -= frame_time;
    }

    /* If we reach a new tile, update position */
    m_move_progress += m_speed * dt;
    if (m_move_progress >= 1.f)
    {
        glm::ivec2 to_position = m_position + m_direction;
        m_move_progress = 0.f;
        m_position = to_position;
    }
}

void Ghost::draw()
{
    glm::ivec2 target = m_position + m_direction;

    /* Interpolate position */
    glm::vec2 draw_position =
        (1.f - m_move_progress) * static_cast<glm::vec2>(m_position) + static_cast<glm::vec2>(target) * m_move_progress;

    /* Draw */
    get_renderer().draw({glm::vec2(TILE_SIZE<float> / 2.f, TILE_SIZE<float> / 2.f) + draw_position * TILE_SIZE<float>,
                         {TILE_SIZE<float>, TILE_SIZE<float>},
                         {1.f, 1.f, 1.f},
                         m_textures[m_direction]});
}

Ghost::EState Ghost::ai_state() const { return m_ai_state; }

void Ghost::set_ai_state(Ghost::EState& ai_state) { m_ai_state = ai_state; }

glm::ivec2 Ghost::position() const { return m_position; }

bool Ghost::is_opposite(glm::ivec2 dir) { return dir == -m_direction; }
}  // namespace pac
