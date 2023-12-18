# Lpr Solver Tool

![Space Invaders Clone](https://www.brettwilsondev.com//assets/images/lpr-solver-tool.webp)

## Description

This is a simple LP Simplex solver tool that makes use of the Primal Simplex and Two-Phase methods, displaying each step and tableau built using C++ and the [Raylib](https://www.raylib.com/) game development library for the app backend. For the GUI frontend, [Dear ImGui](https://github.com/ocornut/imgui) is utilized. The project is configured with CMake and includes a build option for the web using Emscripten.

# Use The tool

## Web Version

Use the tool online [linear Programming Solver tool](https://www.atbdw.com/lpr-solver-tool)

# Building from Source

## Prerequisites
- [cmake](https://cmake.org/)
- [mingw](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/8.1.0/threads-posix/seh/) or [Visual Studio](https://visualstudio.microsoft.com/)
- [Emscripten](https://github.com/emscripten-core/emsdk) (for web build)

## Building for Desktop

```bash
## Clone the repository
git clone https://github.com/BrettWilsonBDW/lpr-solver-tool

## Navigate to the project directory
cd lpr-solver-tool

## Create a build directory
mkdir build

## Navigate to the build directory
cd build

## Run CMake
cmake ..

## Build the project using Visual Studio (you can also use Ninja or MinGW)
cmake --build . --config Release
## or in bash
make

## Run the game
.\Release\SpaceInvadersClone.exe
## or in bash
./Release/SpaceInvadersClone

```
## Building for the Web

```bash
## Ensure Emscripten is installed and configured:
## on windows
cd C:\
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk.bat install latest
./emsdk.bat activate latest --permanent

## on linux
cd /usr/local
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest --permanent


## Navigate to the project directory
cd lpr-solver-tool

## Run the build script for the web
.\build_for_web.bat 
## or in bash 
./build_for_web.sh
```

## Acknowledgments

- [Raylib](https://www.raylib.com/) - A C library for creating simple games.

- [Dear ImGui](https://github.com/ocornut/imgui) - Bloat-free Graphical User interface for C++ with minimal dependencies

- [raylib-cpp-cmake-template](https://github.com/tupini07/raylib-cpp-cmake-template) - Raylib cmake starting point template by [tupini07](https://github.com/tupini07).
