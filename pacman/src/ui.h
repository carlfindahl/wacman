#pragma once

#include <vector>

#include "rendering/renderer.h"

namespace pac
{
namespace ui
{
class TilesetSelector
{
private:
    /* The tileset being edited */
    TextureID m_tileset;

public:
    TilesetSelector(TextureID texture);

    /*!
     * \brief update draws the UI and updates it
     * \param dt is delta time
     */
    void update(float dt);

private:
};
}  // namespace ui
}  // namespace pac
