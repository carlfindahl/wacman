#include "editor_state.h"
#include "state_manager.h"
#include "entity/components.h"
#include "entity/factory.h"
#include "rendering/renderer.h"
#include "game_state.h"
#include "input/input.h"
#include "config.h"
#include "ui.h"

#include <filesystem>

#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
#include <entt/signal/dispatcher.hpp>
#include <entt/meta/factory.hpp>

namespace pac
{
/* The game event queue */
extern entt::dispatcher g_event_queue;

void EditorState::on_enter()
{
    InputDomain editor_domain(true);
    editor_domain.bind_key(GLFW_KEY_ESCAPE, ACTION_BACK);
    editor_domain.bind_live_key(GLFW_MOUSE_BUTTON_LEFT, ACTION_PLACE);
    editor_domain.bind_live_key(GLFW_MOUSE_BUTTON_RIGHT, ACTION_UNDO);
    editor_domain.bind_key(GLFW_MOUSE_BUTTON_MIDDLE, ACTION_CLONE);
    editor_domain.bind_key(GLFW_KEY_T, ACTION_TOGGLE_DEBUG);
    editor_domain.bind_key(GLFW_KEY_E, ACTION_NEXT_TILE);
    editor_domain.bind_key(GLFW_KEY_Q, ACTION_PREV_TILE);
    get_input().push(std::move(editor_domain));

    /* Add Systems */
    m_systems.emplace_back(std::make_unique<AnimationSystem>(*m_context.registry));
    m_systems.emplace_back(std::make_unique<RenderingSystem>(*m_context.registry));

    /* Game overlay for reference */
    m_overlay = get_renderer().load_texture("res/textures/ingame_overlay.png");

    m_level.resize(m_size);

    /* Register event listeners */
    g_event_queue.sink<EvInput>().connect<&EditorState::recieve_key>(this);
    g_event_queue.sink<EvMouseMove>().connect<&EditorState::recieve_mouse>(this);
    m_tileselect_ui.on_select_tile.sink().connect<&EditorState::set_selection>(this);

    /* Fetch available entities */
    load_entity_prototypes();
}

void EditorState::on_exit()
{
    g_event_queue.sink<EvInput>().disconnect<&EditorState::recieve_key>(this);
    g_event_queue.sink<EvMouseMove>().disconnect<&EditorState::recieve_mouse>(this);
    m_tileselect_ui.on_select_tile.sink().disconnect<&EditorState::set_selection>(this);

    get_input().pop();
    m_context.registry->reset();
}

bool EditorState::update(float dt)
{
    draw_ui(dt);
    for (auto& system : m_systems)
    {
        system->update(dt);
    }

    if (m_editor_mode == EMode::EntityPlacement)
    {
        if (m_context.registry->valid(m_entity_about_to_spawn) && m_context.registry->has<CPosition>(m_entity_about_to_spawn))
        {
            m_context.registry->get<CPosition>(m_entity_about_to_spawn).position = m_hovered_tile;
        }
    }
    else
    {
        if (m_context.registry->valid(m_entity_about_to_spawn))
        {
            m_context.registry->destroy(m_entity_about_to_spawn);
        }
    }

    return false;
}

bool EditorState::draw()
{
    m_level.draw();

    /* Only draw tile preview in tile placement mode */
    if (m_editor_mode == EMode::TilePlacement)
    {
        get_renderer().draw({glm::vec2{HALF_TILE + glm::vec2(m_hovered_tile) * TILE_SIZE<float>}, glm::vec2(TILE_SIZE<float>),
                             glm::vec3(.5f, 1.f, 0.5f), get_renderer().get_tileset_texture(m_current_tex)});
    }

    get_renderer().draw({{SCREEN_W / 2.f, SCREEN_H / 2.f}, glm::vec2(SCREEN_W, SCREEN_H), {1.f, 1.f, 1.f}, m_overlay});
    return false;
}

void EditorState::recieve_key(const EvInput& input)
{
    auto& tile = m_level.get_tile(m_hovered_tile);

    switch (input.action)
    {
    case ACTION_BACK: m_context.state_manager->pop(); break;
    case ACTION_PLACE:
        if (m_editor_mode == EMode::TilePlacement)
        {
            tile.type = static_cast<Level::ETileType>(m_current_tex);
            tile.texture = get_renderer().get_tileset_texture(m_current_tex);
        }
        else
        {
            spawn_entity();
        }
        break;
    case ACTION_UNDO:
        if (m_editor_mode == EMode::TilePlacement)
        {
            tile.type = Level::ETileType::Blank;
            tile.texture = {};
        }
        else
        {
            remove_entity();
        }
        break;
    case ACTION_CLONE:
        m_current_tex = tile.texture.frame_number;
        m_tileselect_ui.set_selection(m_current_tex);
        break;
    case ACTION_NEXT_TILE:
        ++m_current_tex;
        m_tileselect_ui.set_selection(m_current_tex);
        break;
    case ACTION_PREV_TILE:
        --m_current_tex;
        m_tileselect_ui.set_selection(m_current_tex);
        break;
    default: break;
    }
}

void EditorState::recieve_mouse(const EvMouseMove& input) { m_hovered_tile = input.position / TILE_SIZE<float>; }

void EditorState::set_selection(unsigned selection) { m_current_tex = selection; }

void EditorState::draw_ui(float dt)
{
    ImGui::Begin("Editor");

    /* Editor Buttons */
    if (ImGui::Button("Animation Editor##Btn"))
    {
        ImGui::OpenPopup("Animation Editor");
    }

    /* Popup Editors */
    if (ImGui::BeginPopupModal("Animation Editor"))
    {
        m_anim_editor.update(dt);
        if (ImGui::Button("Close##Animation"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    /* Saving and Loading */
    ImGui::InputText("Level Name", m_level_name.data(), cgl::size_bytes(m_level_name));
    if (ImGui::Button("Load"))
    {
        m_entities.clear();
        m_level.load(*m_context.lua, *m_context.registry, m_level_name.data());
        load_get_entities();
    }
    ImGui::SameLine();
    if (ImGui::Button("Save") && !m_level_name.empty())
    {
        m_level.save(*m_context.lua, *m_context.registry, m_level_name.data(), m_entities);
    }

    /* Level Properties */
    if (ImGui::DragInt2("Size", glm::value_ptr(m_size), 1.f, 5, 36))
    {
        m_level.resize(m_size);
    }

    ImGui::Separator();

    /* Mode Tabs */
    if (ImGui::BeginTabBar("Modes"))
    {
        if (ImGui::BeginTabItem("Tile Placement"))
        {
            m_editor_mode = EMode::TilePlacement;
            m_tileselect_ui.update(dt);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Entity Placement"))
        {
            m_editor_mode = EMode::EntityPlacement;
            ImGui::BeginChild("EntitySelection");
            /* Show list of all available entities */
            for (const auto& ent : m_entity_prototypes)
            {
                /* When we select a new entity, change selection and... */
                if (ImGui::Selectable(ent.c_str(), ent == m_current_entity))
                {
                    m_current_entity = ent;
                    if (m_context.registry->valid(m_entity_about_to_spawn))
                    {
                        /* .. destroy the existing 'preview entity' and then spawn a new one of the new type */
                        m_context.registry->destroy(m_entity_about_to_spawn);
                    }
                    m_entity_about_to_spawn = EntityFactory(*m_context.registry).spawn(*m_context.lua, m_current_entity);
                }
            }
            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}

void EditorState::load_get_entities()
{
    /* For every entity with meta data, load it into the entity map */
    auto view = m_context.registry->view<CMeta>();
    view.each([this](uint32_t e, const CMeta& meta) {
        glm::ivec2 new_position = {0, 0};

        if (m_context.registry->has<CPosition>(e))
        {
            const auto& pos = m_context.registry->get<CPosition>(e);
            new_position = pos.position;
        }

        /* Find out if this is a new entity or existing one and load accordingly */
        auto itr = std::find_if(m_entities.begin(), m_entities.end(), [&meta](auto&& p) { return p.first == meta.name; });
        if (itr != m_entities.end())
        {
            itr->second.push_back(new_position);
        }
        else
        {
            m_entities.emplace_back(meta.name, std::vector<glm::ivec2>{new_position});
        }
    });
}

void EditorState::load_entity_prototypes()
{
    const auto entity_dir_path = std::filesystem::path(cgl::native_absolute_path("res/entities"));
    for (const auto& entry : std::filesystem::directory_iterator(entity_dir_path))
    {
        m_entity_prototypes.push_back(entry.path().filename().stem().string());
    }
    m_current_entity = m_entity_prototypes.back();
}

void EditorState::spawn_entity()
{
    /* Can not place an entity where there is a wall */
    if (m_level.get_tile(m_hovered_tile).type != Level::ETileType::Blank)
    {
        GFX_INFO("Can not place entity on an occupied tile.");
        return;
    }

    bool occupied = false;

    /* Ensure no two entities with position components are on the same tile */
    m_context.registry->view<CPosition>().each([this, &occupied](uint32_t e, const CPosition& pos) {
        if (e != m_entity_about_to_spawn && pos.position == m_hovered_tile)
        {
            occupied = true;
        }
    });

    /* If it is not occupied, then we can spawn it by changing the entity about to spawn */
    if (!occupied)
    {
        /* Look if entity type exists and then push it back in entity position vector */
        auto pos = std::find_if(m_entities.begin(), m_entities.end(), [this](auto&& p) { return p.first == m_current_entity; });
        if (pos != m_entities.end())
        {
            pos->second.push_back(m_hovered_tile);
        }
        /* Otherwise create new entry */
        else
        {
            m_entities.emplace_back(m_current_entity, std::vector<glm::ivec2>{m_hovered_tile});
        }
        m_entity_about_to_spawn = EntityFactory(*m_context.registry).spawn(*m_context.lua, m_current_entity);
    }
}

void EditorState::remove_entity()
{
    /* Search for entities to delete (and then remove from entity lookup if matching) */
    m_context.registry->view<CPosition>().each([this](uint32_t e, const CPosition& pos) {
        if (pos.position == m_hovered_tile)
        {
            /* Then look for corresponding entry in the save-data vector */
            auto itr = std::find_if(m_entities.begin(), m_entities.end(),
                                    [this](auto&& pair) { return pair.first == m_current_entity; });

            /* If found in there, delete it */
            if (itr != m_entities.end() && itr->second.size() == 1u)
            {
                m_entities.erase(itr);
            }
            /* Otherwise look for corresponding position for that entity and remove that entry instead */
            else
            {
                auto erase_itr =
                    std::find_if(itr->second.cbegin(), itr->second.cend(), [&pos](auto&& vec) { return pos.position == vec; });
                if (erase_itr != itr->second.end())
                {
                    itr->second.erase(erase_itr);
                }
            }

            /* Delete entity */
            m_context.registry->destroy(e);
        }
    });
}

}  // namespace pac
