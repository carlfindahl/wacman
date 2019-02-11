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
    m_pacman = std::make_unique<Pacman>(glm::ivec2(13, 25));
}

Level::Level(std::string_view fp) : Level() { load(fp); }

void Level::update(float dt)
{
    m_pacman->update(dt);

    /* Update Pacman in relation to world */

    if (m_pacman->current_direction() != m_pacman->desired_direction())
    {
        const auto& target_tile =
            m_tiles[m_pacman->m_position.y + m_pacman->desired_direction().y][m_pacman->m_position.x + m_pacman->desired_direction().x];

        if (target_tile.type != ETileType::Wall)
        {
            m_pacman->m_current_direction = m_pacman->m_desired_direction;
        }
    }
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

}  // namespace pac
