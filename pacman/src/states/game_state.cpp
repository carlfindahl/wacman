#include "game_state.h"
#include "entity/components.h"
#include "audio/sound_manager.h"
#include "entity/movement_system.h"
#include "entity/rendering_system.h"
#include "entity/animation_system.h"
#include "state_manager.h"
#include "pause_state.h"
#include "input/input.h"
#include "config.h"

#include <gfx.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <entt/signal/dispatcher.hpp>

namespace pac
{
extern entt::dispatcher g_event_queue;

GameState::GameState(GameContext owner) : State(owner), m_level(owner) {}

void GameState::on_enter()
{
    create_prototypes();
    add_systems();

    m_music_id = get_sound().play("theme", true);

    m_overlay = get_renderer().load_texture("res/ingame_overlay.png");

    m_level.load("res/level0");
    auto pac = m_prototypes.at("pacman").create(m_registry);
    m_registry.get<CPosition>(pac).position = glm::ivec2(3, 3);
    m_registry.get<CMovement>(pac).desired_direction = glm::ivec2(1, 0);
    m_registry.get<CInput>(pac).actions = {{ACTION_MOVE_NORTH,
                                            [pac, this] {
                                                m_registry.get<CMovement>(pac).desired_direction = {0, -1};
                                            }},
                                           {ACTION_MOVE_EAST,
                                            [pac, this] {
                                                m_registry.get<CMovement>(pac).desired_direction = {1, 0};
                                            }},
                                           {ACTION_MOVE_SOUTH,
                                            [pac, this] {
                                                m_registry.get<CMovement>(pac).desired_direction = {0, 1};
                                            }},
                                           {ACTION_MOVE_WEST, [pac, this] {
                                                m_registry.get<CMovement>(pac).desired_direction = {-1, 0};
                                            }}};

    InputDomain game_input(true);
    game_input.bind_key(GLFW_KEY_ESCAPE, ACTION_BACK);
    game_input.bind_key(GLFW_KEY_P, ACTION_PAUSE);
    game_input.bind_key(GLFW_KEY_UP, ACTION_MOVE_NORTH);
    game_input.bind_key(GLFW_KEY_RIGHT, ACTION_MOVE_EAST);
    game_input.bind_key(GLFW_KEY_DOWN, ACTION_MOVE_SOUTH);
    game_input.bind_key(GLFW_KEY_LEFT, ACTION_MOVE_WEST);

    get_input().push(std::move(game_input));

    g_event_queue.sink<EvInput>().connect<&GameState::recieve>(this);
}

void GameState::on_exit()
{
    g_event_queue.sink<EvInput>().disconnect<&GameState::recieve>(this);
    get_sound().stop(m_music_id);
    get_input().pop();
}

bool GameState::update(float dt)
{
    g_event_queue.update();

    m_level.update(dt);
    for (auto& system : m_systems)
    {
        system->update(dt, m_registry);
    }
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
    pacman.set<CMovement>(glm::ivec2{}, glm::ivec2{}, 3.f, 0.f);
    pacman.set<CCollision>();
    pacman.set<CAnimationSprite>(robin_hood::unordered_map<std::string, TextureID>{
        {"up", pacman_up}, {"dn", pacman_dn}, {"lt", pacman_lt}, {"rt", pacman_rt}});
    pacman.get<CAnimationSprite>().active_animation = pacman_rt;
    pacman.set<CInput>();

    m_prototypes.emplace("pacman", std::move(pacman));

    /* Food Prototype */
    auto tileset_tex = get_renderer().load_animation_texture("res/textures/tileset.png", 0, 0, 25, 25, 4, 20);
    tileset_tex.frame_number = 18;

    auto food = entt::prototype{m_registry};
    food.set<CPosition>();
    food.set<CSprite>(tileset_tex, glm::vec3(1.f));
    food.set<CPickup>(50);

    m_prototypes.emplace("food", std::move(food));

    /* Strawberry Prototype */
    tileset_tex.frame_number = 15;
    auto strawberry = entt::prototype{m_registry};
    strawberry.set<CPosition>();
    strawberry.set<CSprite>(tileset_tex, glm::vec3(1.f));
    strawberry.set<CPickup>(250);

    m_prototypes.emplace("strawberry", std::move(strawberry));

    /* Banana Properties */
    tileset_tex.frame_number = 16;
    auto banana = entt::prototype{m_registry};
    banana.set<CPosition>();
    banana.set<CSprite>(tileset_tex, glm::vec3(1.f));
    banana.set<CPickup>(250);

    m_prototypes.emplace("banana", std::move(banana));

    /* Orange Properties */
    tileset_tex.frame_number = 17;
    auto orange = entt::prototype{m_registry};
    orange.set<CPosition>();
    orange.set<CSprite>(tileset_tex, glm::vec3(1.f));
    orange.set<CPickup>(250);

    m_prototypes.emplace("orange", std::move(orange));
}

void GameState::add_systems()
{
    m_systems.emplace_back(std::make_unique<MovementSystem>(m_level));
    m_systems.emplace_back(std::make_unique<AnimationSystem>());
    m_systems.emplace_back(std::make_unique<RenderingSystem>());
}

void GameState::recieve(const EvInput& input)
{
    GFX_DEBUG("Recieve event #%d", input.action);
    switch (input.action)
    {
    case ACTION_BACK: m_context.state_manager->pop(); break;
    case ACTION_PAUSE: m_context.state_manager->push<PauseState>(m_context); break;
    default: break;
    }
}
}  // namespace pac
