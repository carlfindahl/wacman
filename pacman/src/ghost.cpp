#include "ghost.h"

namespace pac
{
Ghost::Ghost()
{
    /* Load Ghost textures */
    m_textures.emplace(glm::ivec2{0, -1}, get_renderer().load_animation_texture("res/pacman.png", 296, 7 + 1 * 70, 70, 70, 2, 2));
    m_textures.emplace(glm::ivec2{0, 1}, get_renderer().load_animation_texture("res/pacman.png", 296, 7 + 0 * 70, 70, 70, 2, 2));
    m_textures.emplace(glm::ivec2{-1, 0}, get_renderer().load_animation_texture("res/pacman.png", 296, 7 + 2 * 70, 70, 70, 2, 2));
    m_textures.emplace(glm::ivec2{1, 0}, get_renderer().load_animation_texture("res/pacman.png", 296, 7 + 3 * 70, 70, 70, 2, 2));
}

Ghost::Ghost(glm::ivec2 position) {}

void Ghost::update(float dt) {}

void Ghost::draw() {}

bool Ghost::is_opposite(glm::ivec2 dir) { return dir == -m_direction; }
}  // namespace pac
