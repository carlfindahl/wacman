#pragma once

#include <vector>
#include <string_view>

namespace pac
{
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

    void update(float dt);

    void draw();

    void load(std::string_view fp);
};
}  // namespace pac
