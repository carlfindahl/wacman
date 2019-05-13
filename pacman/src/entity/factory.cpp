#include "factory.h"
#include "rendering/renderer.h"
#include "components.h"

namespace pac
{
uint32_t EntityFactory::spawn(sol::state_view& state, const std::string& name)
{
    auto e = m_registry.create();

    /* Map entity names to filepaths */
    if (m_entity_path_map.find(name) == m_entity_path_map.end())
    {
        auto path = find_entity_path(name);
        if (path)
        {
            m_entity_path_map[name] = *path;
        }
        else
        {
            return entt::null;
        }
    }

    /* Get the filepath assosciated with the entity name */
    const auto& filepath = m_entity_path_map.at(name);
    GFX_DEBUG("Making entity from file: %s", filepath.filename().c_str());

    /* Load script, and iterate all keys in table */
    state.script_file(filepath.string());
    sol::table table = state[filepath.filename().stem().string()];
    for (const auto& k : table)
    {
        /* Hash key for faster compares, and then get the component */
        sol::table component = table[k.first.as<const char*>()];

        /* Check all known components, and attach it to the entity */
        if (auto pos = m_component_map.find(k.first.as<std::string>()); pos != m_component_map.end())
        {
            m_component_map.at(k.first.as<std::string>())(state, component, e);
        }
    }
    return e;
}

std::optional<std::filesystem::path> EntityFactory::find_entity_path(const std::string& name)
{
    std::stack<std::filesystem::path> to_visit = {};
    to_visit.push(cgl::native_absolute_path("res/entities"));

    /* Until there are no more subdirectories, or we have found the entity */
    while (!to_visit.empty())
    {
        auto current = to_visit.top();
        to_visit.pop();
        for (auto entry : std::filesystem::directory_iterator(current))
        {
            /* If it is a directory, store it for future visits */
            if (entry.is_directory())
            {
                to_visit.push(entry.path());
            }
            else if (entry.is_regular_file())
            {
                if (entry.path().filename().string() == name + ".lua")
                {
                    return entry.path();
                }
            }
        }
    }

    GFX_WARN("Could not find entity'%s', ensure spelling is correct, or that entity exists!", name.c_str());
    return std::nullopt;
}

void EntityFactory::make_sprite_component(sol::state_view& state, const sol::table& comp, uint32_t e)
{
    m_registry.assign<CSprite>(e, get_renderer().get_tileset_texture(comp["index"]), comp["tint"]);
}

void EntityFactory::make_animsprite_component(sol::state_view& state, const sol::table& comp, uint32_t e) {}

void EntityFactory::make_position_component(sol::state_view& state, const sol::table& comp, uint32_t e)
{
    m_registry.assign<CPosition>(e, glm::ivec2{comp["x"], comp["y"]});
}

void EntityFactory::make_movement_component(sol::state_view& state, const sol::table& comp, uint32_t e) {}

void EntityFactory::make_player_component(sol::state_view& state, const sol::table& comp, uint32_t e) {}

void EntityFactory::make_input_component(sol::state_view& state, const sol::table& comp, uint32_t e) {}

void EntityFactory::make_pickup_component(sol::state_view& state, const sol::table& comp, uint32_t e)
{
    m_registry.assign<CPickup>(e, comp["score"]);
}

void EntityFactory::make_collision_component(sol::state_view& state, const sol::table& comp, uint32_t e) {}

}  // namespace pac
