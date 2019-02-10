#include "level.h"
#include "rendering/renderer.h"
#include <config.h>

#include <regex>
#include <sstream>

#include <gfx.h>
#include <cglutil.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace pac
{
Level::Level(std::string_view fp) { load(fp); }

void Level::update(float dt) {}

void Level::draw()
{
    auto& r = get_renderer();
    for (auto y = 0u; y < m_tiles.size(); ++y)
    {
        for (auto x = 0u; x < m_tiles[y].size(); ++x)
        {
            const Tile& t = m_tiles[y][x];
            glm::vec3 col{};

            /* Draw Food */
            if (t.type == ETileType::Food)
            {
                col = {0.85f, 0.77f, 0.68f};
                r.draw({{12.5f + x * TILE_SIZE<float>, 12.5f + y * TILE_SIZE<float>},
                        {TILE_SIZE<float> / 5.f, TILE_SIZE<float> / 5.f},
                        col,
                        {}});
            }
            /* Draw walls */
            else if (t.type == ETileType::Wall)
            {
                col = {0.05f, 0.05f, 0.35f};
                r.draw({{12.5f + x * TILE_SIZE<float>, 12.5f + y * TILE_SIZE<float>},
                        {TILE_SIZE<float>, TILE_SIZE<float>},
                        col,
                        {}});
            }
        }
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
        unsigned tmp_val;
        for (auto& col : row)
        {
            level_stream >> tmp_val;
            col.type = tmp_val == 2u ? ETileType::Food : static_cast<ETileType>(tmp_val);
        }
    }
}

}  // namespace pac
