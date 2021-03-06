# Wacman 2019
 
 ![screenshot](https://i.imgur.com/mrMDEoR.png)
 
### Version History

Currently the latest version is 1.1.2.

 * **1.1.2:** Updated EnTT to latest master version. No gameplay changes.
 * **1.1.1:** Add AudioSystem. This will play sounds on food pickup and on ghost / player death etc. More events too.
 * **1.1.0:** Level Editor, Refactored entire gamplay, basic LUA support, multiple levels and high scores per level...
 * **1.0.2:** Add Post processing effects
 * **1.0.0:** Final game with final music sounds and final logic / gameplay adjustments
 * **0.9.0:** Except final tweaks, has all major gameplay features, but not final audio.
 * **0.1.0:** Every version before that

### Build and Run

#### Build Requirements
 * C++ 17 Compliant Compiler
     * `std::filesystem`
     * `std::string_view`
 * OpenGL 4.5 Compliant Driver
 * [EnTT 3.1](https://github.com/skypjack/entt)
 * [sol2 v3.0](https://github.com/ThePhD/sol2/) 
 * [GLFW 3.2](http://glfw.org/)
 * [GLM](https://glm.g-truc.net/0.9.9/index.html)
 * [OpenAL 1.1](https://www.openal.org/)
 * [Lua](https://www.lua.org/download.html)

GLFW, GLM and OpenAL are required to play. I use `find_package` to locate these libraries so on Linux it should be a matter of installing them via the terminal and then running CMake. On Windows you should specify these directories manually unless you have installed the libraries in a way that they can be found on the `PATH` variable. EnTT and sol2 will attempt to `find_package` but if not, CMake will download the required versions from GitHub and use that local version in the build using `FetchContent`.

#### Building

Run CMake and generate a project, preferably in a `build/` directory. Compile the program with your chosen compiler for your OS/IDE. The external dependencies (GLFW, GLM and OpenAL) will be built automatically by CMake in this step too. Then on Linux, just execute the `pacman` file. If you use Visual Studio the relative directories are weird when running so you must manually copy the `res` folder into the same folder as your executable. This does not apply for VSCode or other generators on Windows.

The application works with relative paths where the relativity always starts from the path of the executable on your hard drive, and not your working directory.

> Remember to build in **Release mode** to avoid being spammed with Debug info and overlays.


#### Running

The game has multiple levels you can play through. It attempts to be as true to the original as possible, with a few tweaks and a more modern feel overall. For example, the theme is remixed and due to the new levels things work differently. Also, the Ghost AI is not 100% like the original. Check out the `Help / Credits` section of the Main Menu in order to see more info.

When you play, your goal is to eat all the tiny food objects without dying. When you do, you win. A high score is recorded locally and as long as you play on the same PC, you can compete with others. The high scores are per level, so if you are terrible at one level, perhaps you will shine doing another one. (*Future idea: Sync high scores online*)

### Sound Licensing
All sound effects are home-made using [SFXR](http://www.drpetter.se/project_sfxr.html) or recorded live and are CC0, public domain now.

The remixed theme was made by my cousin, [Andreas Findahl](https://soundcloud.com/andreasfindahl) on request by me. Check him out, he makes good stuff, and can not be used elsewhere without their permission.
