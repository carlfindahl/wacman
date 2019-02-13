#pragma once

#include "ghost.h"
#include "pacman.h"
#include "rendering/renderer.h"

#include <vector>
#include <memory>
#include <string_view>

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
    /* The level tileset texture */
    TextureID m_tileset_texture = {};

    /* The tiles in the level (basically the map) */
    std::vector<std::vector<Tile>> m_tiles = {};

    /* Pacman, starts bottom center */
    std::unique_ptr<Pacman> m_pacman = nullptr;

    /* Ghost vector */
    std::vector<Ghost> m_ghosts = {};

public:
    Level();

    Level(std::string_view fp);

    /*!
     * \brief update update the state of tiles and the level
     * \param dt is the delta time
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
    void load(std::string_view fp);

    const Tile& get_tile(glm::ivec2 coordinate) const;

    std::vector<glm::ivec2> get_neighbours(glm::ivec2 pos) const;

private:
    void update_pacman();

    void update_ghost(float dt, Ghost& g);
};
}  // namespace pac
