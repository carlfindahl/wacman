#include "factory.h"
#include "rendering/renderer.h"

#include <entt/meta/factory.hpp>

namespace pac
{
EntityFactory::EntityFactory(entt::registry& registry) : m_registry(registry) {}

entt::entity EntityFactory::spawn(sol::state_view& state, const std::string& name)
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

    /* Finally add a meta data component since it was created by a factory */
    m_registry.assign<CMeta>(e, filepath.filename().stem().string());

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
                    GFX_DEBUG("Found entity path: %s", entry.path().c_str());
                    return entry.path();
                }
            }
        }
    }

    GFX_WARN("Could not find entity'%s', ensure spelling is correct, or that entity exists!", name.c_str());
    return std::nullopt;
}

void EntityFactory::make_sprite_component(sol::state_view& state, const sol::table& comp, entt::entity e)
{
    GFX_DEBUG("Adding Sprite Component");
    m_registry.assign<CSprite>(e, CSprite{get_renderer().get_tileset_texture(comp["index"]),
                                          glm::vec3{comp["tint"][1], comp["tint"][2], comp["tint"][3]}});
}

void EntityFactory::make_animsprite_component(sol::state_view& state, const sol::table& comp, entt::entity e)
{
    /* Prepare data */
    robin_hood::unordered_map<std::string, TextureID> anims{};
    sol::table sprites = comp["sprites"];

    /* For each sprite defined, add and load it's animation */
    sprites.for_each([&anims](sol::object k, sol::object v) {
        GFX_DEBUG("Loading animation sprite {%s}", k.as<const char*>());

        auto tbl = v.as<sol::table>();

        /* Load anim texture based on information */
        auto tex = get_renderer().load_animation_texture("res/textures/" + tbl["file"].get<std::string>(),
                                                         tbl["startX"].get<int>(), tbl["startY"].get<int>(), tbl["width"],
                                                         tbl["height"], tbl["cols"], tbl["length"]);

        anims.emplace(k.as<std::string>(), tex);
    });

    /* Finally create animation sprite based on loaded data */
    m_registry.assign<CAnimationSprite>(e, anims, glm::vec3{comp["tint"][1], comp["tint"][2], comp["tint"][3]},
                                        anims[comp["starting"]], 0.f, comp["fps"]);
}

void EntityFactory::make_ai_component(sol::state_view& state, const sol::table& comp, entt::entity e)
{
    m_registry.assign<CAI>(e);
}

void EntityFactory::make_position_component(sol::state_view& state, const sol::table& comp, entt::entity e)
{
    GFX_DEBUG("Position Component at (%d, %d)", comp["x"].get<int>(), comp["y"].get<int>());
    m_registry.assign<CPosition>(e, CPosition{glm::ivec2{comp["x"], comp["y"]}, glm::ivec2{comp["x"], comp["y"]}});
}

void EntityFactory::make_movement_component(sol::state_view& state, const sol::table& comp, entt::entity e)
{
    m_registry.assign<CMovement>(e, glm::ivec2{0}, glm::ivec2{0}, comp["speed"], 0.f);
}

void EntityFactory::make_player_component(sol::state_view& state, const sol::table& comp, entt::entity e)
{
    m_registry.assign<CPlayer>(e, get_renderer().get_tileset_texture(comp["icon"]), comp["lives"].get<int>(), 0, 0.f, 0);
}

void EntityFactory::make_input_component(sol::state_view& state, const sol::table& comp, entt::entity e)
{
    robin_hood::unordered_map<Action, sol::function> actions{};
    for (auto& [k, v] : comp)
    {
        actions.emplace(k.as<Action>(), v.as<sol::function>());
    }

    m_registry.assign<CInput>(e, std::move(actions));
}

void EntityFactory::make_pickup_component(sol::state_view& state, const sol::table& comp, entt::entity e)
{
    GFX_DEBUG("Adding Pikcup Component");
    m_registry.assign<CPickup>(e, comp["score"].get<int>());
}

void EntityFactory::make_collision_component(sol::state_view& state, const sol::table& comp, entt::entity e)
{
    m_registry.assign<CCollision>(e);
}

}  // namespace pac
