#include "reflect.h"
#include "entity/events.h"
#include "rendering/renderer.h"
#include "entity/components.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <entt/meta/factory.hpp>

namespace pac
{
void reflect_common_data()
{
    /* Reflect Vector Types from GLM */
    entt::reflect<glm::ivec2>("ivec2").ctor<int, int>().data<&glm::ivec2::x>("x").data<&glm::ivec2::y>("y");
    entt::reflect<glm::vec2>("vec2").ctor<float, float>().ctor<glm::ivec2>().data<&glm::vec2::x>("x").data<&glm::vec2::y>("y");

    entt::reflect<glm::ivec3>("ivec3")
        .ctor<int, int, int>()
        .data<&glm::ivec3::x>("x")
        .data<&glm::ivec3::y>("y")
        .data<&glm::ivec3::y>("z");

    entt::reflect<glm::vec3>("vec3")
        .ctor<float, float, float>()
        .ctor<glm::ivec3>()
        .data<&glm::vec3::x>("x")
        .data<&glm::vec3::y>("y")
        .data<&glm::vec3::y>("z");

    /* Reflect Texture ID */
    entt::reflect<TextureID>("TextureID")
        .data<&TextureID::frame_number>("frame_number")
        .data<&TextureID::frame_count>("frame_count")
        .data<&TextureID::array_index>("array_index");
}

void reflect_components()
{
    /* Position */
    entt::reflect<CPosition>("Position").data<&CPosition::position>("position").data<&CPosition::spawn>("spawn");

    /* Movement */
    entt::reflect<CMovement>("Movement")
        .data<&CMovement::speed>("speed")
        .data<&CMovement::progress>("progress")
        .data<&CMovement::current_direction>("current_direction")
        .data<&CMovement::desired_direction>("desired_direction");

    /* Teleporter */
    entt::reflect<CTeleporter>("Teleporter")
        .data<&CTeleporter::target>("target")
        .data<&CTeleporter::out_direction>("out_direction");
}

void reflect_events()
{
    /* Reflect Action Enum */
    entt::reflect<Action>("Action")
        .data<Action::ACTION_QUIT>("ACTION_QUIT")
        .data<Action::ACTION_BACK>("ACTION_BACK")
        .data<Action::ACTION_TOGGLE_DEBUG>("ACTION_TOGGLE_DEBUG")
        .data<Action::ACTION_MOVE_NORTH>("ACTION_MOVE_NORTH")
        .data<Action::ACTION_MOVE_EAST>("ACTION_MOVE_EAST")
        .data<Action::ACTION_MOVE_SOUTH>("ACTION_MOVE_SOUTH")
        .data<Action::ACTION_MOVE_WEST>("ACTION_MOVE_WEST")
        .data<Action::ACTION_CONFIRM>("ACTION_CONFIRM")
        .data<Action::ACTION_CANCEL>("ACTION_CANCEL")
        .data<Action::ACTION_PAUSE>("ACTION_PAUSE")
        .data<Action::ACTION_PLACE>("ACTION_PLACE")
        .data<Action::ACTION_UNDO>("ACTION_UNDO")
        .data<Action::ACTION_CLONE>("ACTION_CLONE")
        .data<Action::ACTION_NEXT_TILE>("ACTION_NEXT_TILE")
        .data<Action::ACTION_PREV_TILE>("ACTION_PREV_TILE")
        .data<Action::ACTION_NEXT_ENTITY>("ACTION_NEXT_ENTITY")
        .data<Action::ACTION_PREV_ENTITY>("ACTION_PREV_ENTITY");

    /* Reflect Input Event */
    entt::reflect<EvInput>("EvInput").data<&EvInput::action>("action");

    /* Reflect Mouse Event */
    entt::reflect<EvMouseMove>("EvMouseMove").data<&EvMouseMove::delta>("delta").data<&EvMouseMove::position>("position");

    /* Reflect movement event */
    entt::reflect<EvEntityMoved>("EvEntityMoved")
        .data<&EvEntityMoved::entity>("entity")
        .data<&EvEntityMoved::direction>("direction")
        .data<&EvEntityMoved::new_position>("new_position");
}

void reflect_all()
{
    reflect_common_data();
    reflect_components();
    reflect_events();
}

}  // namespace pac
