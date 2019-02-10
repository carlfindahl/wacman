#pragma once

#include <vector>
#include <string_view>

namespace pac
{
/*!
 * \brief The Level class is responsible for loading and parsing the level format as well as updating state related to
 * interaction with tiles.
 */
class Level
{
private:
    enum class ETileType
    {
        Food,
        Wall,
        Blank,
        Powerup
    };

    struct Tile
    {
        ETileType type = ETileType::Blank;
    };

    std::vector<std::vector<Tile>> m_tiles = {};

public:
    Level() = default;

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
};
}  // namespace pac
