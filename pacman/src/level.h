#pragma once
#include "common.h"
#include "rendering/renderer.h"

#include <chrono>
#include <vector>
#include <memory>
#include <cstdint>
#include <string_view>

#include <robinhood/robinhood.h>
#include <sol/state_view.hpp>
#include <entt/entity/registry.hpp>

namespace pac
{
/*!
 * \brief The Level class is responsible for loading and parsing the level format as well as updating state related to
 * interaction with tiles.
 */
class Level
{
public:
    /*!
     * \brief The ETileType enum specifies the various tiles that exist in the game
     */
    enum class ETileType : int32_t
    {
        Blank = -1,
        Wall,  // 0-14
        Strawberry = 15,
        Banana,
        Orange,
        Food,
        GhostKiller
    };

    /*!
     * \brief The Tile struct contains data needed to draw and know the type of each tile
     */
    struct Tile
    {
        ETileType type = ETileType::Blank;
        TextureID texture = {};
    };

private:
    using seconds = std::chrono::duration<float>;

    /* The tiles in the level (basically the map) */
    std::vector<std::vector<Tile>> m_tiles = {};

    /* The level tileset texture */
    TextureID m_tileset_texture = {};

    /* Score on this level */
    int32_t m_score = 0u;

    /* Context in which level exists */
    GameContext m_context{};

public:
    Level() = default;

    Level(GameContext context);

    /* Level editor can freely change the level */
    friend class EditorState;

    /*!
     * \brief update update the state of tiles and the level
     * \param dt is the delta time
     * \return game status
     */
    void update(float dt);

    /*!
     * \brief draw draws the level (food, tiles and blank tiles
     */
    void draw();

    /*!
     * \brief load a level at the given relative file path
     * \param fp is the relative (to the executable dir) file path of the level file
     */
    void load(sol::state_view& state_view, entt::registry& reg, std::string_view level_name);

    /*!
     * \brief save saves the level to a file
     * \param fp is the relative filepath to save at
     */
    void save(sol::state_view& state_view, const entt::registry& reg, std::string_view level_name,
              const std::vector<std::pair<std::string, glm::ivec2>>& entities);

    /*!
     * \brief get_tile returns the tile at the given coordinate
     * \param coordinate is the requested coordinate of the tile
     * \return the tile at the given coordinate
     */
    Tile& get_tile(glm::ivec2 coordinate);
    const Tile& get_tile(glm::ivec2 coordinate) const;

    /*!
     * \brief will_collide checks if an entity at the given position moveing in the given direction will collide with the level
     * \param pos is the position of the entity
     * \param direction is the direction of the entity
     * \return true if a collision will happen on the next tile, false otherwise
     */
    bool will_collide(glm::ivec2 pos, glm::ivec2 direction) const;

    /*!
     * \brief get_neighbours gets all non-wall neighbouring tiles of the tile at pos
     * \param pos the position of the tile to get the neighbours (NESW) of
     * \return A vector of tile coordinates
     */
    std::vector<glm::ivec2> get_neighbours(glm::ivec2 pos) const;

    /*!
     * \brief score returns current score
     */
    unsigned score() const;

private:
    bool bounds_check(glm::ivec2 pos) const;

    /*!
     * \brief resize changes the size of the level without altering it's contents (unless it is downscaled, in which case data is
     * lost)
     * \param new_size is the new size of the level
     */
    void resize(glm::ivec2 new_size);

    /*!
     * \brief save_to_file saves the current levels lua state to the levels.lua file
     * \param levels_table is the table that holds all the levels
     * \note this is used inside the save function after we have saved the level to the current lua state
     */
    void save_to_file(sol::table levels_table);
};
}  // namespace pac
