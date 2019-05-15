#include "level.h"
#include "pathfinding.h"
#include "entity/factory.h"
#include "audio/sound_manager.h"
#include "states/state_manager.h"
#include "states/respawn_state.h"
#include "states/game_over_state.h"
#include "config.h"

#include <regex>
#include <fstream>
#include <iterator>
#include <algorithm>

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
    for (auto y = 0ul; y < m_tiles.size(); ++y)
    {
        for (auto x = 0ul; x < m_tiles[y].size(); ++x)
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
    reg.reset();
    EntityFactory factory(reg);
    sol::table entities = level_data["entities"];

    for (const auto& [_, entity] : entities)
    {
        sol::table entity_data = entity.as<sol::table>();

        /* Spawn based on name, and then move to position if entity has a position component */
        auto e = factory.spawn(state_view, entity_data["name"]);
        if (reg.has<CPosition>(e))
        {
            reg.get<CPosition>(e).position = {entity_data["x"], entity_data["y"]};
        }
    }
}

void Level::save(sol::state_view& state_view, const entt::registry& reg, std::string_view level_name,
                 const std::vector<std::pair<std::string, glm::ivec2>>& entities)
{
    /* First get access to a table for the level to save. Clear it up front in case it already holds some data */
    state_view.script_file(cgl::native_absolute_path("res/levels.lua"));
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
            if (coltile.type == ETileType::Blank)
            {
                tiles.push_back(-1);
            }
            else
            {
                tiles.push_back(coltile.texture.frame_number);
            }
        }
    }

    /* Then write that information */
    level_data["tiles"] = tiles;

    /* Finally do the same for all entities */
    unsigned next_idx = 1u;
    sol::table entity_data = level_data.create("entities");
    for (const auto& [k, v] : entities)
    {
        auto tbl = entity_data.create(next_idx++);
        tbl["name"] = k;
        tbl["x"] = v.x;
        tbl["y"] = v.y;
    }

    /* Save the new LUA state to file for future loading */
    save_to_file(levels);
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

void Level::save_to_file(sol::table levels_table)
{
    /* Now we must write back the entire levels table to a file for future read back */
    std::ofstream ofile{cgl::native_absolute_path("res/levels.lua")};
    ofile << "levels = {\n\t";

    /* Outer loop is each level */
    for (const auto& [k, v] : levels_table)
    {
        ofile << k.as<std::string>() << " = {\n\t\t";

        /* Inner loop is all level parameters and data */
        const auto current_level = v.as<sol::table>();
        ofile << "w = " << current_level["w"].get<int>() << ",\n\t\t"
              << "h = " << current_level["h"].get<int>() << ",\n\t\t"
              << "tiles = { ";

        /* Copy vector to file */
        const auto& tiledata = current_level["tiles"].get<std::vector<int>>();
        std::copy(tiledata.cbegin(), tiledata.cend(), std::ostream_iterator<int>(ofile, ", "));
        ofile << "},\n\t\t";

        /* Copy entities to file */
        ofile << "entities = {\n\t\t\t";
        for (const auto& [idx, ent] : current_level["entities"].get<sol::table>())
        {
            sol::table entity_data = ent.as<sol::table>();

            /* Write index (of array) then each data member required to properly create the entity */
            ofile << "[" << idx.as<int>() << "] = {\n\t\t\t\t";
            ofile << "name = " << '"' << entity_data["name"].get<std::string>() << '"' << ",\n\t\t\t\t"
                  << "x = " << entity_data["x"].get<int>() << ",\n\t\t\t\t"
                  << "y = " << entity_data["y"].get<int>() << "\n\t\t\t},\n\t\t\t";
        }
        ofile << "}\n\t},\n\t";
    }
    ofile << "}\n";
}

}  // namespace pac
