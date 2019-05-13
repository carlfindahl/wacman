#pragma once

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
    using ComponentFn = void (EntityFactory::*)(sol::state_view&, const sol::table&, uint32_t);

    /* Factory registry */
    entt::registry& m_registry;

    /* Cached entity paths */
    robin_hood::unordered_map<std::string, std::filesystem::path> m_entity_path_map{};

    /* Map that maps hashed component names to component creation functions */
    robin_hood::unordered_map<std::string, ComponentFn> m_component_map = {{"Sprite", &EntityFactory::make_sprite_component},
                                                                           {"Position", &EntityFactory::make_position_component},
                                                                           {"Pickup", &EntityFactory::make_pickup_component}};

public:
    EntityFactory(entt::registry& registry) : m_registry(registry) {}

    /*!
     * \brief spawn spawns a new entity from a resource file
     * \param state is the lua state where the entity is defined
     * \param name is the name of the entity
     * \return the newly spawned entity
     */
    uint32_t spawn(sol::state_view& state, const std::string& name);

private:
    /*!
     * \brief find_entity_path finds the path of the .lua file where the entity data is stored
     * \param name is the name of the entity to look for
     * \return the path if any
     */
    std::optional<std::filesystem::path> find_entity_path(const std::string& name);

    /* Factory functions for each component type */
    void make_sprite_component(sol::state_view& state, const sol::table& comp, uint32_t e);
    void make_animsprite_component(sol::state_view& state, const sol::table& comp, uint32_t e);
    void make_position_component(sol::state_view& state, const sol::table& comp, uint32_t e);
    void make_movement_component(sol::state_view& state, const sol::table& comp, uint32_t e);
    void make_player_component(sol::state_view& state, const sol::table& comp, uint32_t e);
    void make_input_component(sol::state_view& state, const sol::table& comp, uint32_t e);
    void make_pickup_component(sol::state_view& state, const sol::table& comp, uint32_t e);
    void make_collision_component(sol::state_view& state, const sol::table& comp, uint32_t e);
};

}  // namespace pac
