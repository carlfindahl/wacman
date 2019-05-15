#pragma once

#include "rendering/renderer.h"

#include <vector>

#include <entt/signal/sigh.hpp>

namespace pac
{
namespace ui
{
class TilesetSelector
{
private:
    /* The tileset being edited */
    TextureID m_tileset;

    /* Current selected */
    unsigned m_selected = 0u;

public:
    TilesetSelector();
    TilesetSelector(TextureID texture);

    /*!
     * \brief update draws the UI and updates it
     * \param dt is delta time
     */
    void update(float dt);

    /*!
     * \brief set_selection sets the selection of this tileset selector
     * \param s is the new selection
     */
    void set_selection(unsigned s);

    /* Signal for when a tile is selected */
    entt::sigh<void(unsigned)> on_select_tile{};
};
}  // namespace ui
}  // namespace pac
