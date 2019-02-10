#include "game.h"
#include <config.h>

#include <GLFW/glfw3.h>

int main()
{
    glfwInit();

    const auto title_string = std::string("OpenGL Pacman ") + pac::VERSION_STRING;
    pac::Game game(title_string, {700u, 900u});
    game.run();

    glfwTerminate();
    return 0;
}
