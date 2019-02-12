#include "level.h"
#include <config.h>

#include <regex>
#include <sstream>

#include <gfx.h>
#include <cglutil.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace pac
{
Level::Level()
{
    m_tileset_texture = get_renderer().load_animation_texture("res/tileset.png", 0, 0, 25, 25, 4, 20);
    m_pacman = std::make_unique<Pacman>(glm::ivec2(0, 16));

    for (int i = 1; i < 5; ++i)
    {
        m_ghosts.emplace_back(glm::ivec2{i, 3});
    }
}

Level::Level(std::string_view fp) : Level() { load(fp); }

void Level::update(float dt)
{
    /* Update all ghosts, first internally, then with level context */
    for (auto& ghost : m_ghosts)
    {
        ghost.update(dt);
        update_ghost(ghost);
    }

    /* First update Pacman, and then update him in relation to this level (with collision and tile awareness) */
    m_pacman->update(dt);
    update_pacman();
}

void Level::draw()
{
    auto& r = get_renderer();
    for (auto y = 0u; y < m_tiles.size(); ++y)
    {
        for (auto x = 0u; x < m_tiles[y].size(); ++x)
        {
            const Tile& t = m_tiles[y][x];

            /* Skip blank tiles */
            if (t.type == ETileType::Blank)
            {
                continue;
            }

            /* Draw tile */
            r.draw({{12.5f + x * TILE_SIZE<float>, 12.5f + y * TILE_SIZE<float>},
                    {TILE_SIZE<float>, TILE_SIZE<float>},
                    {1.f, 1.f, 1.f},
                    t.texture});
        }
    }

    m_pacman->draw();
    for (auto& ghost : m_ghosts)
    {
        ghost.draw();
    }
}

void Level::load(std::string_view fp)
{
    GFX_INFO("Loading level %s", fp.data());

    /* Read level file data */
    const auto level = cgl::read_entire_file(fp.data());
    std::istringstream level_stream(level);

    /* Read level size from stream. NOTE: I mostly use regex here for fun since it's the first time I am using the <regex>
     * header, so it's a bit experimental, but it works and is actually quite nice, so yeah! */
    auto pat = std::regex(R"((\d+)x(\d+))");
    std::smatch matches{};

    /* Map size should always be in first line, so we get it out of the stream to pattern match */
    std::string first_line = {};
    std::getline(level_stream, first_line);

    if (!std::regex_match(first_line.cbegin(), first_line.cend(), matches, pat) || matches.empty())
    {
        GFX_ERROR("Failed to find level size in level file (%s) with format (WxH).", fp.data());
    }

    /* Get sub-matches from regex match object and convert (parantheses in expression correspond to each sub-match) */
    glm::ivec2 level_size = {};
    level_size.x = std::stoi(matches[1]);
    level_size.y = std::stoi(matches[2]);

    /* Now that we know size of level, reserve space in vectors and fill in the tiles */
    m_tiles.resize(level_size.y);
    for (auto& row : m_tiles)
    {
        row.resize(level_size.x);
        int tmp_val;
        for (auto& col : row)
        {
            level_stream >> tmp_val;

            /* If it is not a tile, then move on */
            if (tmp_val == -1)
            {
                col.type = ETileType::Blank;
                continue;
            }

            /* Use texture ID, but assign frame number from the value in the map */
            col.texture = m_tileset_texture;
            col.texture.frame_number = tmp_val;

            /* Assign based on value */
            if (tmp_val < 14)
            {
                col.type = ETileType::Wall;
            }
            else
            {
                /* This works since the Enum values have been set to match the tile index in the tileset for the level */
                col.type = static_cast<ETileType>(tmp_val);
            }
        }
    }
}

Level::Tile& Level::get_tile(glm::ivec2 coordinate)
{
    GFX_ASSERT(coordinate.y >= 0 && coordinate.y < static_cast<int>(m_tiles.size()), "Y Coordinate out of bounds!");
    GFX_ASSERT(coordinate.x >= 0 && coordinate.x < static_cast<int>(m_tiles[coordinate.y].size()), "X Coordinate out of bounds!");
    return m_tiles[coordinate.y][coordinate.x];
}

void Level::update_pacman()
{
    /* Teleport Pacman across map if going into edge : TODO (make more generic around bounds) */
    if (m_pacman->m_position == glm::ivec2{0, 16} && m_pacman->current_direction() == glm::ivec2{-1, 0})
    {
        m_pacman->m_position = {27, 16};
    }
    else if (m_pacman->m_position == glm::ivec2{27, 16} && m_pacman->current_direction() == glm::ivec2{1, 0})
    {
        m_pacman->m_position = {0, 16};
    }

    /* Make sure we can not turn into adjacent walls */
    if (m_pacman->current_direction() != m_pacman->desired_direction() && m_pacman->m_move_progress < 0.15f)
    {
        const auto& target_tile = get_tile(m_pacman->m_position + m_pacman->desired_direction());
        if (target_tile.type != ETileType::Wall)
        {
            m_pacman->m_current_direction = m_pacman->m_desired_direction;

            /* In Pacman Dossier, it is said that Pacman get's a speed boost around corners. This simulates that.
             * And also stops player from doing 180 degree turns! */
            m_pacman->m_move_progress += 0.15f;
        }
    }

    /* Make sure we are not moving through an existing wall */
    if (get_tile(m_pacman->m_position + m_pacman->current_direction()).type == ETileType::Wall)
    {
        m_pacman->m_move_progress = 0.f;
    }
}

void Level::update_ghost(Ghost& g)
{
    /* Do regular AI (Chasing, Scattering or Scared) */
    switch (g.ai_state())
    {
    case Ghost::EState::Scared: break;
    case Ghost::EState::Chasing: break;
    case Ghost::EState::Scattering: break;
    default: break;
    }

    /* Pathfind to Pacman with A* */

    /* Kill or be killed by Pacman if overlap */
    if (g.position() == m_pacman->m_position)
    {
        GFX_INFO("Pacman has been conusmed by a Ghost, and is dead!");
    }
}

}  // namespace pac
