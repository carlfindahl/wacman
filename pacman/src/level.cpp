#include "level.h"
#include "pathfinding.h"
#include "entity/factory.h"
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
#include <entt/entity/snapshot.hpp>

namespace pac
{
Level::Level(GameContext context) : m_context(context)
{
    m_tileset_texture = get_renderer().load_animation_texture("res/textures/tileset.png", 0, 0, 25, 25, 4, 21);
}

void Level::update(float dt)
{
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

void Level::load(sol::state_view& state_view, entt::registry& reg, std::string_view level_name)
{
    GFX_INFO("Loading level %s", level_name.data());

    /* Read level file data */
    state_view.script_file(cgl::native_absolute_path("res/levels.lua"));
    sol::table level_data = state_view["levels"][level_name];

    /* Reisze level to level size */
    resize({level_data["w"], level_data["h"]});

    /* Load the tile information */
    const auto& tiles = level_data["tiles"].get<std::vector<int>>();

    /* Then use this tile data to generate the level tile format */
    for (auto y = 0ul; y < m_tiles.size(); ++y)
    {
        for (auto x = 0ul; x < m_tiles[y].size(); ++x)
        {
            auto& level_tile = m_tiles[y][x];
            auto tile_type = tiles[y * level_data["w"].get<int>() + x];

            /* It's either a blank tile or it is some kind of wall */
            if (tile_type != -1)
            {
                level_tile.type = ETileType::Wall;
                level_tile.texture = get_renderer().get_tileset_texture(tiles[y * m_tiles[y].size() + x]);
            }
            else
            {
                level_tile.type = ETileType::Blank;
            }
        }
    }

    /* Process entities by key / value */
    sol::table entities = level_data["entities"];
    EntityFactory factory(reg);
    for (auto& [k, v] : entities)
    {
        auto e = factory.spawn(state_view, k.as<std::string>());
        if (reg.has<CPosition>(e))
        {
            reg.get<CPosition>(e).position = v.as<glm::ivec2>();
        }
    }
}

void Level::save(sol::state_view state_view, const entt::registry& reg, std::string_view level_name,
                 robin_hood::unordered_map<std::string, glm::ivec2> entities)
{
    /* First get access to a table for the level to save. Clear it up front in case it already holds some data */
    sol::table levels = state_view["levels"];
    sol::table level_data = levels.create(level_name);
    level_data.clear();

    /* Then start by writing size information */
    level_data["w"] = m_tiles[0].size();
    level_data["h"] = m_tiles.size();

    /* Extract tiles into a single vector */
    std::vector<int> tiles{};
    for (const auto& rowtile : m_tiles)
    {
        for (const auto& coltile : rowtile)
        {
            tiles.push_back(coltile.texture.frame_number);
        }
    }

    /* Then write that information */
    level_data["tiles"] = tiles;

    /* Finally do the same for all entities */
    sol::table entity_data = level_data.create("entities");
    for (const auto& [k, v] : entities)
    {
        entity_data[k] = std::vector<int>{v.x, v.y};
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

bool Level::will_collide(glm::ivec2 pos, glm::ivec2 direction) const { return get_tile(pos + direction).type == ETileType::Wall; }

unsigned Level::score() const { return m_score; }

bool Level::bounds_check(glm::ivec2 pos) const
{
    return pos.y >= 0 && pos.y < static_cast<int>(m_tiles.size()) && pos.x >= 0 &&
           pos.x < static_cast<int>(m_tiles[pos.y].size());
}

void Level::resize(glm::ivec2 new_size)
{
    m_tiles.resize(new_size.y);
    for (auto& xvec : m_tiles)
    {
        xvec.resize(new_size.x);
    }
}

}  // namespace pac
