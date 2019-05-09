#include "level.h"
#include "pathfinding.h"
#include "audio/sound_manager.h"
#include "states/state_manager.h"
#include "states/respawn_state.h"
#include "states/game_over_state.h"
#include "config.h"

#include <regex>
#include <sstream>

#include <gfx.h>
#include <cglutil.h>
#include <imgui/imgui.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace pac
{
Level::Level(GameContext context) : m_context(context)
{
    m_tileset_texture = get_renderer().load_animation_texture("res/tileset.png", 0, 0, 25, 25, 4, 20);
}

void Level::update(float dt)
{
    /* We win */
    if (m_remaining_food == 0)
    {
        m_context.state_manager->push<GameOverState>(m_context, m_score, "YOU WIN!");
    }

    /* Use ImGui to display the score */
    ImGui::SetNextWindowSize({100.f, 16.f});
    ImGui::SetNextWindowPos({120.f, 11.f});
    ImGui::Begin("ScoreWindow", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
    ImGui::Text("%d", m_score);
    ImGui::End();
}

void Level::draw()
{
    auto& r = get_renderer();
    for (auto y = 0u; y < m_tiles.size(); ++y)
    {
        for (auto x = 0u; x < m_tiles[y].size(); ++x)
        {
            const Tile& t = m_tiles[y][x];

            /* Only draw non-blank tiles. Most tiles are non-blank so the if is likely to happen. */
            if (t.type != ETileType::Blank)
            {
                /* Draw tile */
                r.draw({{HALF_TILE + x * TILE_SIZE<float>, HALF_TILE + y * TILE_SIZE<float>},
                        {TILE_SIZE<float>, TILE_SIZE<float>},
                        {1.f, 1.f, 1.f},
                        t.texture});
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
    auto lvl_size_pattern = std::regex(R"((\d+)x(\d+))");
    std::smatch matches{};

    /* Map size should always be in first line, so we get it out of the stream to pattern match */
    std::string first_line = {};
    std::getline(level_stream, first_line);

    if (!std::regex_match(first_line.cbegin(), first_line.cend(), matches, lvl_size_pattern) || matches.empty())
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
                if (col.type == ETileType::Food)
                {
                    ++m_remaining_food;
                }
            }
        }
    }

    level_stream.ignore(1);

    /* Matches either Pacman or Ghost followed by an X and Y coordinate with any number of spaces between */
    const auto entity_pattern = std::regex(R"(^(Pacman|Ghost)\s+(\d+)\s+(\d+))");

    /* Parse entities listed after the map */
    std::string entity_line = {};
    while (std::getline(level_stream, entity_line))
    {
        std::smatch match{};
        if (!std::regex_match(entity_line, match, entity_pattern) || match.empty())
        {
            GFX_WARN("The entity (%s) is not correct, or has unsupported entity type!", entity_line.c_str());
            continue;
        }
    }
}

std::vector<glm::ivec2> Level::get_neighbours(glm::ivec2 pos) const
{
    std::vector<glm::ivec2> out{};

    /* Check all directions and make sure they are not walls. If they are not, add them to out vector and return it. */
    if (pos.x > 0 && get_tile(pos + glm::ivec2{-1, 0}).type != ETileType::Wall)
    {
        out.push_back(pos + glm::ivec2{-1, 0});
    }

    if (pos.x < static_cast<int>(m_tiles[pos.y].size() - 1) && get_tile(pos + glm::ivec2{1, 0}).type != ETileType::Wall)
    {
        out.push_back(pos + glm::ivec2{1, 0});
    }

    if (pos.y > 0 && get_tile(pos + glm::ivec2{0, -1}).type != ETileType::Wall)
    {
        out.push_back(pos + glm::ivec2{0, -1});
    }

    if (pos.y < static_cast<int>(m_tiles.size()) && get_tile(pos + glm::ivec2{0, 1}).type != ETileType::Wall)
    {
        out.push_back(pos + glm::ivec2{0, 1});
    }

    return out;
}

Level::Tile& Level::get_tile(glm::ivec2 coordinate)
{
    GFX_ASSERT(coordinate.y >= 0 && coordinate.y < static_cast<int>(m_tiles.size()), "Y Coordinate out of bounds!");
    GFX_ASSERT(coordinate.x >= 0 && coordinate.x < static_cast<int>(m_tiles[coordinate.y].size()), "X Coordinate out of bounds!");
    return m_tiles[coordinate.y][coordinate.x];
}

const Level::Tile& Level::get_tile(glm::ivec2 coordinate) const
{
    GFX_ASSERT(coordinate.y >= 0 && coordinate.y < static_cast<int>(m_tiles.size()), "Y Coordinate out of bounds!");
    GFX_ASSERT(coordinate.x >= 0 && coordinate.x < static_cast<int>(m_tiles[coordinate.y].size()), "X Coordinate out of bounds!");
    return m_tiles[coordinate.y][coordinate.x];
}

unsigned Level::score() const { return m_score; }

bool Level::bounds_check(glm::ivec2 pos) const
{
    return pos.y >= 0 && pos.y < static_cast<int>(m_tiles.size()) && pos.x >= 0 &&
           pos.x < static_cast<int>(m_tiles[pos.y].size());
}

}  // namespace pac
