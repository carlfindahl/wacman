#include "rendering_system.h"
#include "config.h"
#include "components.h"
#include "rendering/renderer.h"

#include <entt/entity/registry.hpp>

namespace pac
{
void RenderingSystem::update(float dt, entt::registry& reg)
{
    /* Draw all immobile sprites */
    auto regular = reg.group<CSprite>(entt::get<CPosition>, entt::exclude<CMovement>);
    regular.each([](auto e, const CSprite& sprite, const CPosition& pos) {
        get_renderer().draw({glm::vec2(pos.position) * TILE_SIZE<float> - TILE_SIZE<float> * 0.5f,
                             glm::vec2(TILE_SIZE<float>, TILE_SIZE<float>), glm::vec3(1.f), sprite.sprite});
    });

    /* Draw mobile sprites */
    auto regular_moving = reg.group<CSprite>(entt::get<CPosition, CMovement>);
    regular_moving.each([](auto e, const CSprite& sprite, const CPosition& pos, const CMovement& move) {
        /* Interpolate, then draw */
        auto interp_pos = glm::vec2(pos.position) + move.progress * glm::vec2(move.current_direction);

        get_renderer().draw({interp_pos * TILE_SIZE<float> - TILE_SIZE<float> * 0.5f,
                             glm::vec2(TILE_SIZE<float>, TILE_SIZE<float>), glm::vec3(1.f), sprite.sprite});
    });

    /* Draw all immobile animated sprites */
    auto regular_anim = reg.group<CAnimationSprite>(entt::get<CPosition>, entt::exclude<CMovement>);
    regular_anim.each([](auto e, const CAnimationSprite& sprite, const CPosition& pos) {
        get_renderer().draw({glm::vec2(pos.position) * TILE_SIZE<float> - TILE_SIZE<float> * 0.5f,
                             glm::vec2(TILE_SIZE<float>, TILE_SIZE<float>), glm::vec3(1.f), sprite.active_animation});
    });

    /* Draw mobile sprites */
    auto regular_moving_anim = reg.group<CAnimationSprite>(entt::get<CPosition, CMovement>);
    regular_moving_anim.each([](auto e, const CAnimationSprite& sprite, const CPosition& pos, const CMovement& move) {
        /* Interpolate, then draw */
        auto interp_pos = glm::vec2(pos.position) + move.progress * glm::vec2(move.current_direction);

        get_renderer().draw({interp_pos * TILE_SIZE<float> - TILE_SIZE<float> * 0.5f,
                             glm::vec2(TILE_SIZE<float>, TILE_SIZE<float>), glm::vec3(1.f), sprite.active_animation});
    });
}
}  // namespace pac
