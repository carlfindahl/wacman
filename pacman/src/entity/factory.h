#pragma once

#include "components.h"

#include <stack>
#include <string>
#include <cstdint>
#include <optional>
#include <filesystem>

#include <gfx.h>
#include <cglutil.h>
#include <robinhood/robinhood.h>
#include <sol/state_view.hpp>
#include <entt/entity/registry.hpp>
#include <entt/signal/delegate.hpp>

namespace pac
{
class EntityFactory
{
private:
    using ComponentFn = std::function<void(sol::state_view&, const sol::table&, entt::entity)>;

    /* Factory registry */
    entt::registry& m_registry;

    /* Cached entity paths */
    robin_hood::unordered_map<std::string, std::filesystem::path> m_entity_path_map{};

    /* Map component names to their creation functions */
    robin_hood::unordered_map<std::string, ComponentFn> m_component_map{
        {"Sprite", [this](sol::state_view& s, const sol::table& t, entt::entity e) { make_sprite_component(s, t, e); }},
        {"Position", [this](sol::state_view& s, const sol::table& t, entt::entity e) { make_position_component(s, t, e); }},
        {"Pickup", [this](sol::state_view& s, const sol::table& t, entt::entity e) { make_pickup_component(s, t, e); }},
        {"Collision", [this](sol::state_view& s, const sol::table& t, entt::entity e) { make_collision_component(s, t, e); }},
        {"Movement", [this](sol::state_view& s, const sol::table& t, entt::entity e) { make_movement_component(s, t, e); }},
        {"Player", [this](sol::state_view& s, const sol::table& t, entt::entity e) { make_player_component(s, t, e); }},
        {"AnimationSprite",
         [this](sol::state_view& s, const sol::table& t, entt::entity e) { make_animsprite_component(s, t, e); }},
        {"AI", [this](sol::state_view& s, const sol::table& t, entt::entity e) { make_ai_component(s, t, e); }},
        {"Input", [this](sol::state_view& s, const sol::table& t, entt::entity e) { make_input_component(s, t, e); }}};

public:
    EntityFactory(entt::registry& registry);

    /*!
     * \brief spawn spawns a new entity from a resource file
     * \param state is the lua state where the entity is defined
     * \param name is the name of the entity
     * \return the newly spawned entity
     */
    entt::entity spawn(sol::state_view& state, const std::string& name);

private:
    /*!
     * \brief find_entity_path finds the path of the .lua file where the entity data is stored
     * \param name is the name of the entity to look for
     * \return the path if any
     */
    std::optional<std::filesystem::path> find_entity_path(const std::string& name);

    /* Factory functions for each component type */
    void make_sprite_component(sol::state_view& state, const sol::table& comp, entt::entity e);
    void make_animsprite_component(sol::state_view& state, const sol::table& comp, entt::entity e);
    void make_ai_component(sol::state_view& state, const sol::table& comp, entt::entity e);
    void make_position_component(sol::state_view& state, const sol::table& comp, entt::entity e);
    void make_movement_component(sol::state_view& state, const sol::table& comp, entt::entity e);
    void make_player_component(sol::state_view& state, const sol::table& comp, entt::entity e);
    void make_input_component(sol::state_view& state, const sol::table& comp, entt::entity e);
    void make_pickup_component(sol::state_view& state, const sol::table& comp, entt::entity e);
    void make_collision_component(sol::state_view& state, const sol::table& comp, entt::entity e);
};

}  // namespace pac
