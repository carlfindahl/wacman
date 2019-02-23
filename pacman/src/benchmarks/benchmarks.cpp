#include "encrypt/vignere_encryptor.h"
#include "states/state_manager.h"
#include "pathfinding.h"
#include "level.h"

#include <benchmark/benchmark.h>

#include <glm/vec2.hpp>

static void bench_pathfinding_bfs(benchmark::State& state)
{
    pac::Level lvl{};
    lvl.load("res/level0");

    glm::ivec2 start = {1, 3};
    glm::ivec2 end = {13, 25};
    for (auto _ : state)
    {
        auto p = pac::Path(lvl, start, end, pac::Path::BFS{});
        benchmark::DoNotOptimize(p);
    }
}

BENCHMARK(bench_pathfinding_bfs);

static void bench_pathfinding_astar(benchmark::State& state)
{
    pac::Level lvl{};
    lvl.load("res/level0");

    glm::ivec2 start = {1, 3};
    glm::ivec2 end = {13, 25};
    for (auto _ : state)
    {
        auto p = pac::Path(lvl, start, end, pac::Path::ASTAR{});
        benchmark::DoNotOptimize(p);
    }
}

BENCHMARK(bench_pathfinding_astar);
