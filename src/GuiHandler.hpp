#pragma once

#include "raylib.h"
// #include "rlgl.h"

#include "imgui.h"
#include "rlImGui.h"

#include "PrimalSolver.hpp"
#include "TwoPhase.hpp"
#include "PrimalTwoPhaseBase.hpp"

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
    void HandelInput();
    void PassInputToSimplex();
    void ConsoleInfo();
    void SetUpTables(PrimalTwoPhaseBase *simplex);
    // void SetUpTables();
    void DisplayTable(std::vector<std::vector<std::vector<float>>> tab, int ctr, bool isPrimalTable = false);
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

    // ImVec4 accentColor(204.0f / 255.0f, 51.0f / 255.0f, 0.0f, 1.0f);

    ImVec4 accentColor{ImVec4(204.0f / 255.0f, 51.0f / 255.0f, 0.0f, 1.0f)};

    std::vector<std::string> objFunctionLabels = {"x1", "x2"};

    std::vector<float> objFunction = {0.0f, 0.0f};

    int objFunctionCtr{2};
    int constraintsCtr{2};

    std::vector<float> tempConstraints = {0.0f, 0.0f, 0.0f, 0.0f};
    std::vector<std::vector<float>> constraints = {tempConstraints, tempConstraints};

    // PrimalTwoPhaseBase *simplex = nullptr;

    bool solve{};
    bool runOnce{};
};
