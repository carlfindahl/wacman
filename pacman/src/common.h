#pragma once

#include <utility>
#include <vector>
#include <string>
#include <cstdint>

#include <glm/vec2.hpp>
#include <sol/state.hpp>
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <robinhood/robinhood.h>

namespace pac
{
class StateManager;

namespace detail
{
/*!
 * \brief The ivec2hash struct is used as a position based hash function for the entity map
 */
struct custom_ivec2_hash
{
    std::size_t operator()(const glm::ivec2& vec) const noexcept
    {
        return static_cast<uint64_t>(vec.x) | (static_cast<uint64_t>(vec.y) << 32u);
    }
};
}  // namespace detail

/*!
 * \brief The Context struct hold relevant context information about the Game that various sub systems might care about.
 */
struct GameContext
{
    StateManager* state_manager = nullptr;
    sol::state* lua = nullptr;
    entt::registry* registry = nullptr;
};

/*!
 * \brief The Entry struct represents a single entry in the high score table
 */
struct ScoreEntry
{
    std::string name = {};
    int score = 0;
};

/*!
 * \brief The EAIState enum describes the current state of an AI in the game
 */
enum class EAIState
{
    Chasing,
    ChasingAhead,
    Searching,
    Scattering,
    Scared,
    Dead
};

/*!
 * \brief wraps a sol function call for use with EnTT
 * \note Thanks @skypjack for the implementation and help
 */
template<typename Ev>
void sol_function_wrapper(sol::function& func, const Ev& event)
{
    func(event);
}

/*!
 * \brief function used to bind lua and entt event connections
 * \note Again, thanks to @skypjack for the tip
 */
template<typename Ev>
entt::scoped_connection lua_binder(entt::dispatcher& disp, sol::function& func)
{
    return disp.sink<Ev>().connect<&sol_function_wrapper<Ev>>(func);
}

/*!
 * \brief load_entries_from_file loads high score entries from highscores.txt in order to display them in the menu
 * \return a vector of entries
 */
robin_hood::unordered_map<std::string, std::vector<ScoreEntry>>
load_high_score_entries_from_file(const char* filepath = "res/highscores.txt");

/*!
 * \brief write_high_score_entries_to_file writes all high scores in the vector to the given file
 * \param entries are the entries to write
 * \param filepath is the file to write to
 */
void write_high_score_entries_to_file(const robin_hood::unordered_map<std::string, std::vector<ScoreEntry>>& entries,
                                      const char* filepath = "res/highscores.txt");

/*!
 * \brief manhattan_distance compute manhattan distance between two points, used in astar as a heuristic for example
 * \param from is point A
 * \param to is point B
 * \return manhattan distance between points A and B
 */
int manhattan_distance(glm::ivec2 from, glm::ivec2 to) noexcept;
}  // namespace pac
