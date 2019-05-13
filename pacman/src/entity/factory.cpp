#include "factory.h"
#include "rendering/renderer.h"

#include <entt/meta/factory.hpp>

namespace pac
{
EntityFactory::EntityFactory(entt::registry& registry) : m_registry(registry)
{
    //    entt::reflect<TextureID>("TextureID")
    //        .data<&TextureID::array_index>("index")
    //        .data<&TextureID::frame_count>("frames")
    //        .data<&TextureID::frame_number>("current_frame");

    //    entt::reflect<glm::ivec2>("ivec2").data<&glm::ivec2::x>("x").data<&glm::ivec2::y>("y");

    //    /* Reflect Components */
    //    entt::reflect<CPosition>("Position").ctor<&detail::make_position_component>().data<&CPosition::position>("position");
    //    entt::reflect<CPickup>("Pickup").ctor<&detail::make_pickup_component>().data<&CPickup::score>("score");
    //    entt::reflect<CSprite>("Sprite").ctor<&detail::make_sprite_component>().data<&CSprite::tint>("tint").data<&CSprite::sprite>(
    //        "sprite");

    //    /* Debug for reflection props */
    //    entt::resolve([](entt::meta_type type) { GFX_DEBUG("Has reflected type: %s", type.name()); });
}

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
        GFX_DEBUG("%s has %s Component.", filepath.filename().c_str(), k.first.as<const char*>());

        /* Hash key for faster compares, and then get the component */
        sol::table component = table[k.first.as<const char*>()];

        /* Check all known components, and attach it to the entity */
        const auto& key = k.first.as<std::string>();
        if (auto fn = m_component_map.find(key); fn != m_component_map.end())
        {
            fn->getSecond()(state, component, e);
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
    GFX_DEBUG("Adding Sprite Component");
    m_registry.assign<CSprite>(e, CSprite{get_renderer().get_tileset_texture(comp["index"]),
                                          glm::vec3{comp["tint"][0], comp["tint"][1], comp["tint"][2]}});
}

void EntityFactory::make_animsprite_component(sol::state_view& state, const sol::table& comp, uint32_t e) {}

void EntityFactory::make_position_component(sol::state_view& state, const sol::table& comp, uint32_t e)
{
    GFX_DEBUG("Position Component at (%d, %d)", comp["x"].get<int>(), comp["y"].get<int>());
    m_registry.assign<CPosition>(e, CPosition{glm::ivec2{comp["x"], comp["y"]}});
}

void EntityFactory::make_movement_component(sol::state_view& state, const sol::table& comp, uint32_t e) {}

void EntityFactory::make_player_component(sol::state_view& state, const sol::table& comp, uint32_t e) {}

void EntityFactory::make_input_component(sol::state_view& state, const sol::table& comp, uint32_t e) {}

void EntityFactory::make_pickup_component(sol::state_view& state, const sol::table& comp, uint32_t e)
{
    GFX_DEBUG("Adding Pikcup Component");
    m_registry.assign<CPickup>(e, CPickup{comp["score"]});
}

void EntityFactory::make_collision_component(sol::state_view& state, const sol::table& comp, uint32_t e) {}

}  // namespace pac
