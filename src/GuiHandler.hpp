#pragma once

#include "raylib.h"
// #include "rlgl.h"

#include "imgui.h"
#include "rlImGui.h"

#include "PrimalSolver.hpp"
#include "TwoPhase.hpp"

#include <iostream>
#include <sstream>
#include <vector>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

// Define a class named ImGuiStreamBuffer that inherits from the std::streambuf class
class ImGuiStreamBuffer : public std::streambuf
{
public:
    virtual int_type overflow(int_type c = traits_type::eof())
    {
        if (c != traits_type::eof())
        {
            buffer += static_cast<char>(c);
            if (c == '\n')
            {
                // Update ImGui window with captured content when a newline is encountered
                consoleBuffer += buffer + '\n';
                buffer.clear();
            }
        }
        return c;
    }

    const std::string &getConsoleBuffer() const
    {
        return consoleBuffer;
    }

    void clearConsoleBuffer()
    {
        consoleBuffer.clear();
    }

private:
    std::string buffer;
    std::string consoleBuffer;
};

class GuiHandler
{
public:
    GuiHandler();
    ~GuiHandler();

    void Init();
    void Unload();
    void ConsoleInfo();
    void SetUpTables();
    void DisplayTable(std::vector<std::vector<std::vector<float>>> tab, int ctr);
    void Draw();
    void Run();

private:
    void OutPutToConsoleWindow();

    // Create an instance of the custom stream buffer
    ImGuiStreamBuffer imguiStreamBuffer;

    // Redirect std::cout to the custom stream buffer
    std::streambuf *coutBuffer = std::cout.rdbuf(&imguiStreamBuffer);

    TwoPhase twoPhase;

    // std::vector<std::vector<std::vector<float>>> tableaus = twoPhase.GetTableaus();

    static void MainLoopHelper(void *userData);
    int screenWidth{};
    int screenHeight{};
};
