#include "game_state.h"
#include "entity/components.h"
#include "audio/sound_manager.h"
#include "state_manager.h"
#include "pause_state.h"
#include "input/input.h"
#include "config.h"

#include <gfx.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <entt/entity/prototype.hpp>

namespace pac
{
GameState::GameState(GameContext owner) : State(owner), m_level(owner) {}

void GameState::on_enter()
{
    m_music_id = get_sound().play("theme", true);

    m_overlay = get_renderer().load_texture("res/ingame_overlay.png");

    InputState game_input(true);
    game_input.bind_key(GLFW_KEY_ESCAPE, [this] { m_context.state_manager->push<PauseState>(m_context); });
    game_input.bind_key(GLFW_KEY_P, [this] { m_context.state_manager->push<PauseState>(m_context); });

    auto& input_manager = get_input();
    input_manager.push(std::move(game_input));

    m_level.load("res/level0");
}

void GameState::on_exit()
{
    get_sound().stop(m_music_id);
    get_input().pop();
}

bool GameState::update(float dt)
{
    m_level.update(dt);
    return false;
}

bool GameState::draw()
{
    m_level.draw();
    get_renderer().draw({{SCREEN_W / 2.f, SCREEN_H / 2.f}, glm::vec2(SCREEN_W, SCREEN_H), {1.f, 1.f, 1.f}, m_overlay});
    return false;
}

void GameState::create_prototypes()
{
    auto pacman_dn = get_renderer().load_animation_texture("res/textures/pacman.png", 0, 0, 70, 70, 4, 4);
    auto pacman_up = get_renderer().load_animation_texture("res/textures/pacman.png", 0, 70, 70, 70, 4, 4);
    auto pacman_lt = get_renderer().load_animation_texture("res/textures/pacman.png", 0, 140, 70, 70, 4, 4);
    auto pacman_rt = get_renderer().load_animation_texture("res/textures/pacman.png", 0, 210, 70, 70, 4, 4);

    /* Pacman Prototype */
    auto pacman = entt::prototype{m_registry};
    pacman.set<CPlayer>();
    pacman.set<CPosition>();
    pacman.set<CMovement>(glm::ivec2{}, glm::ivec2{}, 3.f);
    pacman.set<CCollision>();
    pacman.set<CAnimationSprite>(robin_hood::unordered_map<std::string, TextureID>{
        {"up", pacman_up}, {"dn", pacman_dn}, {"lt", pacman_lt}, {"rt", pacman_rt}});

    /* Food Prototype */
    auto tileset_tex = get_renderer().load_animation_texture("res/textures/tileset.png", 0, 0, 25, 25, 4, 20);
    tileset_tex.frame_number = 18;

    auto food = entt::prototype{m_registry};
    food.set<CPosition>();
    food.set<CSprite>(tileset_tex);
    food.set<CPickup>(50);

    /* Strawberry Prototype */
    tileset_tex.frame_number = 15;
    auto strawberry = entt::prototype{m_registry};
    strawberry.set<CPosition>();
    strawberry.set<CSprite>(tileset_tex);
    strawberry.set<CPickup>(250);

    /* Banana Properties */
    tileset_tex.frame_number = 16;
    auto banana = entt::prototype{m_registry};
    banana.set<CPosition>();
    banana.set<CSprite>(tileset_tex);
    banana.set<CPickup>(250);

    /* Orange Properties */
    tileset_tex.frame_number = 17;
    auto orange = entt::prototype{m_registry};
    orange.set<CPosition>();
    orange.set<CSprite>(tileset_tex);
    orange.set<CPickup>(250);

    /* Wall Properties */
    tileset_tex.frame_number = 17;
    auto wall = entt::prototype{m_registry};
    wall.set<CPosition>();
    wall.set<CSprite>(tileset_tex);
    wall.set<CPickup>(250);
    wall.set<CCollision>();

}
}  // namespace pac
