#pragma once

#include "level.h"
#include "common.h"

#include <stack>
#include <queue>
#include <vector>
#include <chrono>
#include <type_traits>
#include <unordered_map>

#include <glm/vec2.hpp>

namespace pac
{
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
    Path(const Level& graph, glm::ivec2 origin, glm::ivec2 target) : m_creation_time(std::chrono::steady_clock::now())
    {
        pathfind_bfs(graph, origin, target);
    }

    /*!
     * \brief get the next direction from the path
     * \return the next direction to move to get to your target
     */
    glm::ivec2 get()
    {
        if (!m_directions.empty())
        {
            auto val = m_directions.top();
            m_directions.pop();
            return val;
        }

        return {};
    }

    /*!
     * \brief empty returns empty if the Path does not exist
     * \return true if the path is empty
     */
    bool empty() const { return m_directions.empty(); }

    /*!
     * \brief outdated checks if the path is outdated
     * \return true if the path is considered outdated.
     */
    bool outdated() const { return (std::chrono::steady_clock::now() - m_creation_time) > std::chrono::milliseconds(200); }

private:
    void pathfind_bfs(const Level& graph, glm::ivec2 origin, glm::ivec2 target)
    {
        /* Get neighbours and try all paths until we find target, or if we don't the Path will be empty */
        std::queue<glm::ivec2> next_node = {};
        next_node.push(origin);

        std::unordered_map<glm::ivec2, glm::ivec2, detail::custom_ivec2_hash> traceback = {};
        traceback.emplace(origin, origin);

        while (!next_node.empty())
        {
            auto current = next_node.front();
            next_node.pop();

            if (current == target)
            {
                break;
            }

            for (auto next : graph.get_neighbours(current))
            {
                if (traceback.find(next) == traceback.end())
                {
                    next_node.push(next);
                    traceback.emplace(next, current);
                }
            }
        }

        /* Do traceback into stack */
        for (auto it = target; it != origin; it = traceback.at(it))
        {
            m_directions.push(it - traceback.at(it));
        }
    }
};
}  // namespace pac
