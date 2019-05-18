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

void Level::update(float dt) {}

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
        const auto& x_positions = entity_data["x"].get<std::vector<int>>();
        const auto& y_positions = entity_data["y"].get<std::vector<int>>();

        /* For each position, spawn an entity */
        for (auto i = 0u; i < x_positions.size(); ++i)
        {
            auto e = factory.spawn(state_view, entity_data["name"]);
            reg.get<CPosition>(e).position = {x_positions[i], y_positions[i]};
            reg.get<CPosition>(e).spawn = {x_positions[i], y_positions[i]};
        }
    }
}

void Level::save(sol::state_view& state_view, const entt::registry& reg, std::string_view level_name,
                 const std::vector<std::pair<std::string, std::vector<glm::ivec2>>>& entities)
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
        /* Extract X and Y Positions */
        std::vector<int> x_positions{};
        std::vector<int> y_positions{};
        std::transform(v.cbegin(), v.cend(), std::back_inserter(x_positions), [](auto&& e) { return e.x; });
        std::transform(v.cbegin(), v.cend(), std::back_inserter(y_positions), [](auto&& e) { return e.y; });

        /* Write one element per entity type */
        auto tbl = entity_data.create(next_idx++);
        tbl["name"] = k;
        tbl["x"] = x_positions;
        tbl["y"] = y_positions;
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

glm::ivec2 Level::find_closest_intersection(glm::ivec2 start, glm::ivec2 dir) const
{
    /* Get all possible movement directions */
    auto directions = get_neighbours(start);

    /* Delete the reverse direction since we can not move 180 degrees */
    auto e_itr =
        std::remove_if(directions.begin(), directions.end(), [&dir, &start](glm::ivec2 elem) { return (elem - start) == -dir; });
    directions.erase(e_itr, directions.end());

    /* Now choose a direction to move in and go as far as possible in that way */
    std::vector<glm::ivec2> possible_targets = {};
    for (const auto& dir : directions)
    {
        glm::ivec2 movement_vector = dir - start;
        glm::ivec2 target_tile = dir;
        while (bounds_check(target_tile + movement_vector) && get_tile(target_tile + movement_vector).type != ETileType::Wall)
        {
            target_tile += movement_vector;
        }

        possible_targets.push_back(target_tile);
    }

    /* Find one with smallest distance in as few moves as possible */
    std::nth_element(
        possible_targets.begin(), possible_targets.begin(), possible_targets.end(),
        [pos = start](glm::ivec2 a, glm::ivec2 b) { return manhattan_distance(a, pos) < manhattan_distance(b, pos); });

    /* We know this is the point furthest away from pacman after nth-element */
    return possible_targets[0];
}

bool Level::los(glm::ivec2 start, glm::ivec2 end) const
{
    /* If we are next to this tile, always has LOS */
    if (manhattan_distance(start, end) < 2)
    {
        return true;
    }
    /* On diagonals, always false */
    else if (start.x != end.x && start.y != end.y)
    {
        return false;
    }

    /* Find delta between start and end */
    const auto delta = direction(start, end);

    /* Trace from start to end with the delta -> If we hit a wall, there is no LOS */
    for (; start != end; start += delta)
    {
        if (get_tile(start).type == ETileType::Wall)
        {
            return false;
        }
    }

    return true;
}

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

glm::ivec2 Level::direction(glm::ivec2 from, glm::ivec2 to) const
{
    auto diff = glm::sign(to - from);
    return diff;
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
                  << "x = {";

            const auto& x_vec = entity_data["x"].get<std::vector<int>>();
            std::copy(x_vec.cbegin(), x_vec.cend(), std::ostream_iterator<int>(ofile, ", "));
            ofile << "},\n\t\t\t\t"
                  << "y = {";

            const auto& y_vec = entity_data["y"].get<std::vector<int>>();
            std::copy(y_vec.cbegin(), y_vec.cend(), std::ostream_iterator<int>(ofile, ", "));
            ofile << "}\n\t\t\t},\n\t\t\t";
        }
        ofile << "}\n\t},\n\t";
    }
    ofile << "}\n";
}

glm::ivec2 Level::find_sensible_escape_point(glm::ivec2 ghost_pos, glm::ivec2 ghost_dir, glm::ivec2 escape_from_pos)
{
    /* Compute direction to pacman so we can prefer some directions to others */
    const auto pacman_delta = escape_from_pos - ghost_pos;

    /* Get all possible movement directions */
    auto directions = get_neighbours(ghost_pos);

    /* Delete the reverse direction since we can not move 180 degrees */
    auto e_itr = std::remove_if(directions.begin(), directions.end(),
                                [pos = ghost_pos, dir = ghost_dir](glm::ivec2 elem) { return (elem - pos) == -dir; });
    directions.erase(e_itr, directions.end());

    /* Put directions going away from pacman in the front of the collection */
    std::partition(directions.begin(), directions.end(), [&pacman_delta, &ghost_pos](glm::ivec2 v) {
        return (v.x - ghost_pos.x) == -pacman_delta.x || (v.y - ghost_pos.y) == -pacman_delta.y;
    });

    /* Now choose a direction to move in and go as far as possible in that way */
    std::vector<glm::ivec2> possible_targets = {};
    for (const auto& dir : directions)
    {
        glm::ivec2 movement_vector = dir - ghost_pos;
        glm::ivec2 target_tile = dir;
        while (bounds_check(target_tile + movement_vector) && get_tile(target_tile + movement_vector).type != ETileType::Wall)
        {
            target_tile += movement_vector;
        }

        possible_targets.push_back(target_tile);
    }

    /* Find one with largest distance in as few moves as possible */
    std::nth_element(
        possible_targets.begin(), possible_targets.begin(), possible_targets.end(),
        [pos = escape_from_pos](glm::ivec2 a, glm::ivec2 b) { return manhattan_distance(a, pos) > manhattan_distance(b, pos); });

    /* We know this is the point furthest away from pacman after nth-element */
    return possible_targets[0];
}

}  // namespace pac
