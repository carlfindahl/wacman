#include "game.h"

#include <GLFW/glfw3.h>

int main()
{
    glfwInit();

    pac::Game game("OGLPM v0.0.0", {800u, 800u});
    game.run();

    glfwTerminate();
    return 0;
}
