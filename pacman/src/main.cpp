#include "game.h"
#include "config.h"

#include <string>

#include <GLFW/glfw3.h>

#ifdef NDEBUG
#ifdef PACMAN_USE_BENCHMARKS
#include <benchmark/benchmark.h>
#endif
#endif

using namespace std::string_literals;

int main(int argc, char* argv[])
{
#ifdef NDEBUG
#ifdef PACMAN_USE_BENCHMARKS
    if (argc == 2)
    {
        if (argv[1] == "bench"s)
        {
            benchmark::Initialize(&argc, argv);
            benchmark::RunSpecifiedBenchmarks();
            return 0;
        }
    }
#endif
#endif

    /* Run game if we are not benchmarking */
    glfwInit();

    const auto title_string = "OpenGL Pacman "s + pac::VERSION_STRING;
    pac::Game game(title_string, {pac::SCREEN_W, pac::SCREEN_H});
    game.run();

    glfwTerminate();
    return 0;
}
