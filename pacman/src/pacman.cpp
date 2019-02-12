#include "pacman.h"
#include <config.h>
#include <input.h>

#include <gfx.h>
#include <GLFW/glfw3.h>

namespace pac
{
/* Simply delegates work */
Pacman::Pacman() : Pacman({0, 0}) {}

pac::Pacman::Pacman(glm::ivec2 position) : m_position(position)
{
    /* Load pacman textures */
    m_textures.emplace(EDirection::North, get_renderer().load_animation_texture("res/pacman.png", 7, 7 + 1 * 70, 70, 70, 4, 4));
    m_textures.emplace(EDirection::South, get_renderer().load_animation_texture("res/pacman.png", 7, 7 + 0 * 70, 70, 70, 4, 4));
    m_textures.emplace(EDirection::West, get_renderer().load_animation_texture("res/pacman.png", 7, 7 + 2 * 70, 70, 70, 4, 4));
    m_textures.emplace(EDirection::East, get_renderer().load_animation_texture("res/pacman.png", 7, 7 + 3 * 70, 70, 70, 4, 4));

    /* Add pacman input to the stack (if more time, do this in scripting instead of hard coding it) */
    input::InputState pacman_input{};
    pacman_input.set_binding(GLFW_KEY_W, [this] { this->turn(EDirection::North); });
    pacman_input.set_binding(GLFW_KEY_A, [this] { this->turn(EDirection::West); });
    pacman_input.set_binding(GLFW_KEY_S, [this] { this->turn(EDirection::South); });
    pacman_input.set_binding(GLFW_KEY_D, [this] { this->turn(EDirection::East); });
    pacman_input.set_binding(GLFW_KEY_UP, [this] { this->turn(EDirection::North); });
    pacman_input.set_binding(GLFW_KEY_LEFT, [this] { this->turn(EDirection::West); });
    pacman_input.set_binding(GLFW_KEY_DOWN, [this] { this->turn(EDirection::South); });
    pacman_input.set_binding(GLFW_KEY_RIGHT, [this] { this->turn(EDirection::East); });

    input::get_input().push(std::move(pacman_input));
}

void Pacman::turn(EDirection new_direction)
{
    if (!is_opposite(new_direction))
    {
        m_desired_direction = new_direction;
    }
}

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

glm::ivec2 Pacman::current_direction() const
{
    switch (m_current_direction)
    {
    case EDirection::North: return {0, -1}; break;
    case EDirection::South: return {0, 1}; break;
    case EDirection::East: return {1, 0}; break;
    case EDirection::West: return {-1, 0}; break;
    default: return {0, 0}; break;
    }
}

glm::ivec2 Pacman::desired_direction() const
{
    switch (m_desired_direction)
    {
    case EDirection::North: return {0, -1}; break;
    case EDirection::South: return {0, 1}; break;
    case EDirection::East: return {1, 0}; break;
    case EDirection::West: return {-1, 0}; break;
    default: return {0, 0}; break;
    }
};

bool Pacman::is_opposite(EDirection dir)
{
    switch (m_current_direction)
    {
    case EDirection::North: return dir == EDirection::South; break;
    case EDirection::South: return dir == EDirection::North; break;
    case EDirection::East: return dir == EDirection::West; break;
    case EDirection::West: return dir == EDirection::East; break;
    default: return false; break;
    }
}

}  // namespace pac
