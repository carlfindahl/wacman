#include "reflect.h"
#include "level.h"
#include "entity/events.h"
#include "rendering/renderer.h"
#include "entity/components.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <entt/meta/factory.hpp>
#include <entt/core/hashed_string.hpp>

namespace pac
{
void reflect_common_data()
{
    /* Reflect Vector Types from GLM */
    entt::meta<glm::ivec2>().type("ivec2"_hs).ctor<int, int>().data<&glm::ivec2::x>("x"_hs).data<&glm::ivec2::y>("y"_hs);
    entt::meta<glm::vec2>()
        .type("vec2"_hs)
        .ctor<float, float>()
        .ctor<glm::ivec2>()
        .data<&glm::vec2::x>("x"_hs)
        .data<&glm::vec2::y>("y"_hs);

    entt::meta<glm::ivec3>()
        .type("ivec3"_hs)
        .ctor<int, int, int>()
        .data<&glm::ivec3::x>("x"_hs)
        .data<&glm::ivec3::y>("y"_hs)
        .data<&glm::ivec3::z>("z"_hs);

    entt::meta<glm::vec3>()
        .type("vec3"_hs)
        .ctor<float, float, float>()
        .ctor<glm::vec3>()
        .data<&glm::vec3::x>("x"_hs)
        .data<&glm::vec3::y>("y"_hs)
        .data<&glm::vec3::z>("z"_hs);

    /* Reflect Texture ID */
    entt::meta<TextureID>()
        .type("TextureID"_hs)
        .data<&TextureID::frame_number>("frame_number"_hs)
        .data<&TextureID::frame_count>("frame_count"_hs)
        .data<&TextureID::array_index>("array_index"_hs);

    /* Reflect TP Destination */
    entt::meta<Level::TeleportDestination>()
        .type("TeleportDestination"_hs)
        .data<&Level::TeleportDestination::from>("from"_hs)
        .data<&Level::TeleportDestination::position>("position"_hs)
        .data<&Level::TeleportDestination::direction>("direction"_hs);
}

void reflect_components()
{
    /* Position */
    entt::meta<CPosition>().type("Position"_hs).data<&CPosition::position>("position"_hs).data<&CPosition::spawn>("spawn"_hs);

    /* Movement */
    entt::meta<CMovement>()
        .type("Movement"_hs)
        .data<&CMovement::speed>("speed"_hs)
        .data<&CMovement::progress>("progress"_hs)
        .data<&CMovement::current_direction>("current_direction"_hs)
        .data<&CMovement::desired_direction>("desired_direction"_hs);
}

void reflect_events()
{
    /* Reflect Action Enum */
    entt::meta<Action>()
        .type("Action"_hs)
        .data<Action::ACTION_QUIT>("ACTION_QUIT"_hs)
        .data<Action::ACTION_BACK>("ACTION_BACK"_hs)
        .data<Action::ACTION_TOGGLE_DEBUG>("ACTION_TOGGLE_DEBUG"_hs)
        .data<Action::ACTION_MOVE_NORTH>("ACTION_MOVE_NORTH"_hs)
        .data<Action::ACTION_MOVE_EAST>("ACTION_MOVE_EAST"_hs)
        .data<Action::ACTION_MOVE_SOUTH>("ACTION_MOVE_SOUTH"_hs)
        .data<Action::ACTION_MOVE_WEST>("ACTION_MOVE_WEST"_hs)
        .data<Action::ACTION_CONFIRM>("ACTION_CONFIRM"_hs)
        .data<Action::ACTION_CANCEL>("ACTION_CANCEL"_hs)
        .data<Action::ACTION_PAUSE>("ACTION_PAUSE"_hs)
        .data<Action::ACTION_PLACE>("ACTION_PLACE"_hs)
        .data<Action::ACTION_UNDO>("ACTION_UNDO"_hs)
        .data<Action::ACTION_CLONE>("ACTION_CLONE"_hs)
        .data<Action::ACTION_NEXT_TILE>("ACTION_NEXT_TILE"_hs)
        .data<Action::ACTION_PREV_TILE>("ACTION_PREV_TILE"_hs)
        .data<Action::ACTION_NEXT_ENTITY>("ACTION_NEXT_ENTITY"_hs)
        .data<Action::ACTION_PREV_ENTITY>("ACTION_PREV_ENTITY"_hs);

    /* Reflect Input Event */
    entt::meta<EvInput>().type("EvInput"_hs).data<&EvInput::action>("action"_hs);

    /* Reflect Mouse Event */
    entt::meta<EvMouseMove>()
        .type("EvMouseMove"_hs)
        .data<&EvMouseMove::delta>("delta"_hs)
        .data<&EvMouseMove::position>("position"_hs);

    /* Reflect movement event */
    entt::meta<EvEntityMoved>()
        .type("EvEntityMoved"_hs)
        .data<&EvEntityMoved::entity>("entity"_hs)
        .data<&EvEntityMoved::direction>("direction"_hs)
        .data<&EvEntityMoved::new_position>("new_position"_hs);
}

void reflect_all()
{
    reflect_common_data();
    reflect_components();
    reflect_events();
}

}  // namespace pac
