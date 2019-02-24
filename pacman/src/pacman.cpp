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
    m_textures.emplace(glm::ivec2(0, -1), get_renderer().load_animation_texture("res/pacman.png", 7, 7 + 1 * 70, 70, 70, 4, 4));
    m_textures.emplace(glm::ivec2(0, 1), get_renderer().load_animation_texture("res/pacman.png", 7, 7 + 0 * 70, 70, 70, 4, 4));
    m_textures.emplace(glm::ivec2(-1, 0), get_renderer().load_animation_texture("res/pacman.png", 7, 7 + 2 * 70, 70, 70, 4, 4));
    m_textures.emplace(glm::ivec2(1, 0), get_renderer().load_animation_texture("res/pacman.png", 7, 7 + 3 * 70, 70, 70, 4, 4));

    /* Add pacman input to the stack (if more time, do this in scripting instead of hard coding it) */
    input::InputState pacman_input{};
    pacman_input.set_binding(GLFW_KEY_W, [this] { turn({0, -1}); });
    pacman_input.set_binding(GLFW_KEY_A, [this] { turn({-1, 0}); });
    pacman_input.set_binding(GLFW_KEY_S, [this] { turn({0, 1}); });
    pacman_input.set_binding(GLFW_KEY_D, [this] { turn({1, 0}); });
    pacman_input.set_binding(GLFW_KEY_UP, [this] { turn({0, -1}); });
    pacman_input.set_binding(GLFW_KEY_LEFT, [this] { turn({-1, 0}); });
    pacman_input.set_binding(GLFW_KEY_DOWN, [this] { turn({0, 1}); });
    pacman_input.set_binding(GLFW_KEY_RIGHT, [this] { turn({1, 0}); });

    input::get_input().push(std::move(pacman_input));
}

Pacman::~Pacman() { input::get_input().pop(); }

void Pacman::turn(glm::ivec2 new_direction)
{
    //    if (!is_opposite(new_direction))
    //    {
    m_desired_direction = new_direction;
    //    }
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
        m_move_progress = 0.f;
        m_position += m_current_direction;
    }
}

void Pacman::draw()
{
    glm::ivec2 target = m_position + m_current_direction;

    /* Interpolate position */
    glm::vec2 draw_position =
        (1.f - m_move_progress) * static_cast<glm::vec2>(m_position) + static_cast<glm::vec2>(target) * m_move_progress;

    /* Draw */
    get_renderer().draw({glm::vec2(TILE_SIZE<float> / 2.f, TILE_SIZE<float> / 2.f) + draw_position * TILE_SIZE<float>,
                         {TILE_SIZE<float>, TILE_SIZE<float>},
                         {1.f, 1.f, 1.f},
                         m_textures[m_current_direction]});

    for (int i = 0; i < m_lives; ++i)
    {
        get_renderer().draw({glm::vec2(TILE_SIZE<float> / 2.f, TILE_SIZE<float> / 2.f) +
                                 glm::vec2(1 * (i + 1), SCREEN_H / TILE_SIZE<int> - 2) * TILE_SIZE<float>,
                             {TILE_SIZE<float>, TILE_SIZE<float>},
                             {1.f, 1.f, 1.f},
                             m_textures[m_current_direction]});
    }
}

glm::ivec2 Pacman::current_direction() const { return m_current_direction; }

glm::ivec2 Pacman::desired_direction() const { return m_desired_direction; };

int Pacman::lives() const { return m_lives; }

bool Pacman::is_opposite(glm::ivec2 dir) { return m_current_direction == -dir; }

}  // namespace pac
