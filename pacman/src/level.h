#pragma once
#include "ghost.h"
#include "pacman.h"
#include "common.h"
#include "rendering/renderer.h"

#include <chrono>
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

    enum class ELevelState : int32_t
    {
        Playing,
        Respawning,
        Won,
        Lost
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

    /* Ghost vector */
    std::vector<Ghost> m_ghosts = {};

    /* Pacman, starts bottom center */
    std::unique_ptr<Pacman> m_pacman = nullptr;

    /* The level tileset texture */
    TextureID m_tileset_texture = {};

    /* Score on this level */
    unsigned m_score = 0u;

    /* Pacman spawn */
    glm::ivec2 m_pacman_spawn = {};

    /* How long will ghosts chase before scattering */
    seconds m_chasetime{10.f};

    /* Number of seconds left before pacman is no longer a killer */
    seconds m_pacman_kill_timer{0.f};

    /* seconds m_strawberry_timer = 60.f; - When it goes to zero, spawn a fruit :D */

    GameContext m_context{};

public:
    Level(GameContext context);

    /*!
     * \brief update update the state of tiles and the level
     * \param dt is the delta time
     * \return game status
     */
    ELevelState update(float dt);

    /*!
     * \brief draw draws the level (food, tiles and blank tiles
     */
    void draw();

    /*!
     * \brief load a level at the given relative file path
     * \param fp is the relative (to the executable dir) file path of the level file
     */
    void load(std::string_view fp);

    /*!
     * \brief get_tile returns the tile at the given coordinate
     * \param coordinate is the requested coordinate of the tile
     * \return the tile at the given coordinate
     */
    Tile& get_tile(glm::ivec2 coordinate);
    const Tile& get_tile(glm::ivec2 coordinate) const;

    /*!
     * \brief get_neighbours gets all non-wall neighbouring tiles of the tile at pos
     * \param pos the position of the tile to get the neighbours (NESW) of
     * \return A vector of tile coordinates
     */
    std::vector<glm::ivec2> get_neighbours(glm::ivec2 pos) const;

    unsigned score() const;

private:
    void update_pacman();

    void update_ghost(float dt, Ghost& g);

    glm::ivec2 find_sensible_escape_point(Ghost& g);
};
}  // namespace pac
