#include "ui.h"

#include <imgui/imgui.h>

namespace pac
{
namespace ui
{
TilesetSelector::TilesetSelector(TextureID texture) : m_tileset(texture) {}

void TilesetSelector::update(float dt)
{
    using namespace ImGui;

    auto frame_tex = m_tileset;
    for (auto i = 0u; i < m_tileset.frame_count; ++i)
    {
        frame_tex.frame_number = i;
        Image((void*)get_renderer().get_texture_for_imgui(frame_tex), {25, 25});
        if ((i + 1) % 4 != 0)
        {
            ImGui::SameLine();
        }
    }
}

}  // namespace ui
}  // namespace pac
