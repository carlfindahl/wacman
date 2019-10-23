#include "game.h"
#include "config.h"

#include <string>

#include <GLFW/glfw3.h>

using namespace std::string_literals;

int main(int argc, char* argv[])
{
    if (glfwInit())
    {
        const auto title_string = "OpenGL Pacman "s + pac::VERSION_STRING;
        pac::Game game(title_string, {pac::SCREEN_W, pac::SCREEN_H});
        game.run();

        glfwTerminate();
    }
    return 0;
}
