#pragma once

#include <stack>
#include <chrono>

#include <glm/vec2.hpp>
#include <cglutil.h>

namespace pac
{
class Level;

/*!
 * \class Path
 * \brief Path represents a stack of directions from an origin to a target.
 */
class Path
{
private:
    /* The directions you must take to get to your target */
    std::stack<glm::ivec2> m_directions = {};

    /* Creation time of this path. Paths will time out after 0.25 seconds */
    std::chrono::steady_clock::time_point m_creation_time = {};

public:
    Path() = delete;

    /*!
     * \brief Construct  a Path from a type of Graph
     */
    Path(const Level& graph, glm::ivec2 origin, glm::ivec2 target);

    Path(const Path&) = default;
    Path(Path&&) = default;
    Path& operator=(const Path&) = default;
    Path& operator=(Path&&) = default;

    CGL_NEVER_INLINE ~Path() noexcept;
    /*!
     * \brief get the next direction from the path
     * \return the next direction to move to get to your target
     */
    glm::ivec2 get();

    /*!
     * \brief empty returns empty if the Path does not exist
     * \return true if the path is empty
     */
    bool empty() const;

    /*!
     * \brief outdated checks if the path is outdated
     * \return true if the path is considered outdated.
     */
    bool outdated() const;

private:
    /*!
     * \brief pathfind_bfs uses breadth first search to find a suitable path
     * \param graph is the graph that can be used to get neighbours
     * \param origin is where we want to pathfind from
     * \param target is the desired target position
     */
    void pathfind_bfs(const Level& graph, glm::ivec2 origin, glm::ivec2 target) noexcept;

    /*!
     * \brief pathfind_astar uses A* to find a suitable path
     * \param graph is the graph that can be used to get neighbours
     * \param origin is where we want to pathfind from
     * \param target is the desired target position
     */
    void pathfind_astar(const Level& graph, glm::ivec2 origin, glm::ivec2 target) noexcept;
};
}  // namespace pac
