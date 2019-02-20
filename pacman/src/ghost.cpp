#include "ghost.h"
#include "pathfinding.h"
#include <config.h>

#include <gfx.h>

namespace pac
{
Ghost::Ghost() : Ghost({0, 0}) {}

Ghost::Ghost(glm::ivec2 position) : m_home(position), m_position(position)
{
    /* Load Ghost textures */
    m_textures.emplace(glm::ivec2{0, 0}, get_renderer().load_animation_texture("res/pacman.png", 296, 7 + 1 * 70, 70, 70, 1, 1));
    m_textures.emplace(glm::ivec2{0, -1}, get_renderer().load_animation_texture("res/pacman.png", 296, 7 + 1 * 70, 70, 70, 2, 2));
    m_textures.emplace(glm::ivec2{0, 1}, get_renderer().load_animation_texture("res/pacman.png", 296, 7 + 0 * 70, 70, 70, 2, 2));
    m_textures.emplace(glm::ivec2{-1, 0}, get_renderer().load_animation_texture("res/pacman.png", 296, 7 + 2 * 70, 70, 70, 2, 2));
    m_textures.emplace(glm::ivec2{1, 0}, get_renderer().load_animation_texture("res/pacman.png", 296, 7 + 3 * 70, 70, 70, 2, 2));
}

Ghost::Ghost(const Ghost& other)
    : m_textures(other.m_textures), m_home(other.m_home), m_position(other.m_position), m_direction(other.m_direction),
      m_move_progress(other.m_move_progress), m_speed(other.m_speed), m_animation_time(other.m_animation_time),
      m_ai_state(other.m_ai_state)
{
    m_path = new Path(*other.m_path);
}

Ghost& Ghost::operator=(const Ghost& other)
{
    if (this == &other)
    {
        return *this;
    }

    if (m_path)
    {
        delete m_path;
    }

    m_textures = other.m_textures;
    m_position = other.m_position;
    m_direction = other.m_direction;
    m_move_progress = other.m_move_progress;
    m_speed = other.m_speed;
    m_animation_time = other.m_animation_time;
    m_ai_state = other.m_ai_state;
    m_home = other.m_home;

    m_path = new Path(*other.m_path);
    return *this;
}

Ghost::Ghost(Ghost&& other) noexcept
    : m_textures(std::move(other.m_textures)), m_home(other.m_home), m_position(other.m_position), m_direction(other.m_direction),
      m_move_progress(other.m_move_progress), m_speed(other.m_speed), m_animation_time(other.m_animation_time),
      m_ai_state(other.m_ai_state)
{
    m_path = other.m_path;
    other.m_path = nullptr;
}

Ghost& Ghost::operator=(Ghost&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    if (m_path)
    {
        delete m_path;
    }

    m_textures = std::move(other.m_textures);
    m_position = other.m_position;
    m_direction = other.m_direction;
    m_move_progress = other.m_move_progress;
    m_speed = other.m_speed;
    m_animation_time = other.m_animation_time;
    m_ai_state = other.m_ai_state;
    m_home = other.m_home;
    m_path = other.m_path;
    other.m_path = nullptr;

    return *this;
}

Ghost::~Ghost() { delete m_path; }

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
        m_move_progress = 0.f;
        m_position += m_direction;

        if (m_path && !m_path->empty())
        {
            m_direction = m_path->get();
        }

        if (m_position == m_home && m_dead)
        {
            m_ai_state = EState::Scared;
            m_dead = false;
        }

        if (m_position == m_home && m_ai_state == EState::Scattering)
        {
            m_ai_state = EState::Chasing;
        }
    }
}

void Ghost::draw(bool blue)
{
    glm::ivec2 target = m_position + m_direction;

    /* Interpolate position */
    glm::vec2 draw_position =
        (1.f - m_move_progress) * static_cast<glm::vec2>(m_position) + static_cast<glm::vec2>(target) * m_move_progress;

    /* Give ghost appropriate color */
    glm::vec3 color = {1.f, 1.f, 1.f};
    if (m_dead)
    {
        color.r = 0.f;
    }
    else if (blue)
    {
        color.g = 0.3f;
        color.r = 0.3f;
        color.b = 1.f;
    }

    /* Draw */
    get_renderer().draw({glm::vec2(TILE_SIZE<float> / 2.f, TILE_SIZE<float> / 2.f) + draw_position * TILE_SIZE<float>,
                         {TILE_SIZE<float>, TILE_SIZE<float>},
                         color,
                         m_textures[m_direction]});
}

Ghost::EState Ghost::ai_state() const { return m_ai_state; }

void Ghost::set_ai_state(Ghost::EState ai_state) { m_ai_state = ai_state; }

glm::ivec2 Ghost::position() const { return m_position; }

void Ghost::set_position(glm::ivec2 pos) { m_position = pos; }

glm::ivec2 Ghost::home() const { return m_home; }

void Ghost::set_path(Path* path_on_heap)
{
    if (m_path)
    {
        delete m_path;
    }

    /* Update path and then immediately update direction */
    m_path = path_on_heap;
    m_direction = m_path->get();
}

bool Ghost::requires_path_update() const { return m_path == nullptr || m_path->outdated(); }

void Ghost::die() { m_dead = true; }

bool Ghost::dead() const { return m_dead; }

bool Ghost::is_opposite(glm::ivec2 dir) { return dir == -m_direction; }
}  // namespace pac
