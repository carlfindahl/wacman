# Pacman 2019 - By Undefined Behaviour


### Group Information

 Carl Findahl - S484369 - [carlhf@stud.ntnu.no](mailto:carlhf@stud.ntnu.no)
 
 ![screenshot](https://i.imgur.com/uj91iLT.png)
 
### Version History

Currently the latest version is 1.0.0. Before that there was only 0.9.0 and 0.1.0. Where 0.9.0 was the almost completed version and 0.1.0 was every version before that as an in-development build.

**1.0.0:** Final game with final music sounds and final logic / gameplay adjustments
**0.9.0:** Except final tweaks, has all major gameplay features, but not final audio.
**0.1.0:** Every version before that

### Build and Run

#### Requirements
 * C++ 17 Compliant Compiler
     * `std::filesystem`
     * `std::string_view`
 * OpenGL 4.5 Compliant Driver
 * [GLFW 3.2](http://glfw.org/)
 * [GLM](https://glm.g-truc.net/0.9.9/index.html)
 * [OpenAL 1.1](https://www.openal.org/) *[optional, but very recommended for audio]* 
 * [Google Benchmark](https://github.com/google/benchmark) *[optional, only if you want to run benchmarks]*

On Linux, CMake should be able to find all of these automatically using `find_package`. On Windows this requires that you have built and installed the required libraries to a folder that is on your system `PATH` environment variable.

#### Building

Run CMake and generate a project, preferably in a `build/` directory. Compile the program with your chosen compiler for your OS/IDE. Then on Linux, just execute the `pacman` file. If you use Visual Studio the relative directories are weird when running so you must manually copy the `res` folder into the same folder as your executable. This does not apply for VSCode on Windows.

The application works with relative paths where the relativity always starts from the path of the executable on your hard drive, and not your working directory.

> Remember to build in **Release mode** to avoid being spammed with Debug info and overlays.


#### Running

The game has all extras except multiple levels at the moment. It attempts to be as true to the original as possible, with a few tweaks and a more modern feel overall. For example, the theme is remixed and there are some interesting new rules. Check out the `Help / Credits` section of the Main Menu in order to see the rules and controls.

When you play, your goal is to eat all the tiny food objects without dying. When you do, you win. A high score is recorded locally and as long as you play on the same PC, you can compete with others. (*Future idea: Sync high scores online*)

### Sound Licensing
All sound effects are home-made using [SFXR](http://www.drpetter.se/project_sfxr.html) or recorded live and are CC0, public domain now.

The remixed theme was made by my cousin, [Andreas Findahl](https://soundcloud.com/andreasfindahl) on request by me. Check him out, he makes good stuff, and can not be used elsewhere without their permission.
