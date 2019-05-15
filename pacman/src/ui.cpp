#include "ui.h"

#include <imgui/imgui.h>

namespace pac
{
namespace ui
{
TilesetSelector::TilesetSelector() : m_tileset(get_renderer().get_tileset_texture(0u)) {}

TilesetSelector::TilesetSelector(TextureID texture) : m_tileset(texture) {}

void TilesetSelector::update(float dt)
{
    using namespace ImGui;

    auto frame_tex = m_tileset;
    for (auto i = 0u; i < m_tileset.frame_count; ++i)
    {
        frame_tex.frame_number = i;

        /* If we click the tileset btn, then signal that we have changed selection */
        if (ImageButton((void*)get_renderer().get_texture_for_imgui(frame_tex), {25, 25}, {0, 0}, {1, 1}, -1, {0, 0, 0, 0},
                        (m_selected == i ? ImVec4{1.f, 1.f, 1.f, 1.f} : ImVec4{0.5f, 0.5f, 0.5f, 0.5f})))
        {
            m_selected = i;
            on_select_tile.publish(static_cast<unsigned>(i));
        }

        if ((i + 1) % 6 != 0)
        {
            ImGui::SameLine();
        }
    }
}

void TilesetSelector::set_selection(unsigned s) { m_selected = s; }

}  // namespace ui
}  // namespace pac
