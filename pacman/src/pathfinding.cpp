#include "pathfinding.h"
#include "level.h"
#include "common.h"

#include <queue>
#include <vector>
#include <type_traits>
#include <unordered_map>

#include <gfx.h>

pac::Path::Path(const pac::Level& graph, glm::ivec2 origin, glm::ivec2 target) : m_creation_time(std::chrono::steady_clock::now())
{
    pathfind_astar(graph, origin, target);
}

pac::Path::~Path() noexcept {}

glm::ivec2 pac::Path::get()
{
    if (!m_directions.empty())
    {
        auto val = m_directions.top();
        m_directions.pop();
        return val;
    }

    return {};
}

bool pac::Path::empty() const { return m_directions.empty(); }

bool pac::Path::outdated() const { return (std::chrono::steady_clock::now() - m_creation_time) > std::chrono::milliseconds(90); }

void pac::Path::pathfind_bfs(const pac::Level& graph, glm::ivec2 origin, glm::ivec2 target) noexcept
{
    /* Get neighbours and try all paths until we find target, or if we don't the Path will be empty */
    std::queue<glm::ivec2> next_node = {};
    next_node.push(origin);

    std::unordered_map<glm::ivec2, glm::ivec2, detail::custom_ivec2_hash> traceback = {};
    traceback[origin] = origin;

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
                traceback[next] = current;
            }
        }
    }

    /* Do traceback into stack */
    for (auto it = target; it != origin; it = traceback.at(it))
    {
        m_directions.push(it - traceback.at(it));
    }
}

void pac::Path::pathfind_astar(const pac::Level& graph, glm::ivec2 origin, glm::ivec2 target) noexcept
{
    /* Get neighbours and try all paths until we find target, or if we don't the Path will be empty */
    using priority_pair = std::pair<int, glm::ivec2>;
    auto priority_func = [](const priority_pair& a, const priority_pair& b) { return a.first > b.first; };

    /* Use a priority queue with heuristic */
    std::priority_queue<priority_pair, std::vector<priority_pair>, decltype(priority_func)> next_node(priority_func);
    next_node.push(priority_pair(0.0, origin));

    /* Keep a traceback map and a cost map */
    std::unordered_map<glm::ivec2, glm::ivec2, detail::custom_ivec2_hash> traceback = {};
    std::unordered_map<glm::ivec2, int, detail::custom_ivec2_hash> cost_so_far = {};
    traceback[origin] = origin;
    cost_so_far[origin] = 0;

    while (!next_node.empty())
    {
        auto current = next_node.top().second;
        next_node.pop();

        if (current == target)
        {
            break;
        }

        /* Apply heuristic while processing the map */
        for (auto next : graph.get_neighbours(current))
        {
            int new_cost = cost_so_far[current] + 1;
            if (cost_so_far.find(next) == cost_so_far.end() || new_cost < cost_so_far[next])
            {
                cost_so_far[next] = new_cost;
                traceback[next] = current;
                next_node.push(priority_pair(new_cost + heuristic(next, target), next));
            }
        }
    }

    /* Do traceback into stack */
    for (auto it = target; it != origin; it = traceback.at(it))
    {
        m_directions.push(it - traceback.at(it));
    }
}

int pac::Path::heuristic(glm::ivec2 from, glm::ivec2 to) const noexcept
{
    return std::abs(from.x - to.x) + std::abs(from.y - to.y);
}
