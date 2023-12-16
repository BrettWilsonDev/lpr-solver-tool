#include "GuiHandler.hpp"

GuiHandler::GuiHandler()
{
    Init();
    Run();
}

GuiHandler::~GuiHandler()
{
    Unload();
}

void GuiHandler::Init()
{
    screenWidth = 1920 / 2;
    screenHeight = 1080 / 2;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Liner Programming Solver Tool");
    // SetTargetFPS(144);
    rlImGuiSetup(true);
    ImGui::GetIO().IniFilename = "";

    // ImFontConfig fontConfig;
    // fontConfig.FontDataOwnedByAtlas = false;
    // ImGui::GetIO().Fonts->Clear();
    // ImGui::GetIO().Fonts->AddFontFromFileTTF("../Roboto.ttf", 18, &fontConfig);
}

void GuiHandler::Unload()
{
    rlImGuiShutdown();

    std::cout.rdbuf(coutBuffer);

    CloseWindow();
}

void GuiHandler::SetUpTables()
{
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.0f, 10.0f));
    for (int i = 0; i < static_cast<int>(twoPhase.GetTableaus().size()); i++)
    {
        DisplayTable(twoPhase.GetTableaus(), i);
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
    }
    ImGui::PopStyleVar();
}

void GuiHandler::DisplayTable(std::vector<std::vector<std::vector<float>>> tab, int ctr)
{
    std::vector<std::vector<float>> tableau = tab[ctr];
    if (ImGui::BeginTable("table1", static_cast<int>(tableau[0].size() + 1),
                          ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersOuterH |
                              ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersOuterV |
                              ImGuiTableFlags_SizingFixedFit |
                              ImGuiTableFlags_NoKeepColumnsVisible | ImGuiTableFlags_ScrollX,
                          ImVec2(0, (30 * static_cast<int>(tableau[0].size())))))
    {
        for (int row = 0; row < static_cast<int>(tableau.size()); row++)
        {
            ImGui::TableNextRow();
            for (int column = 0; column < static_cast<int>(tableau[row].size()); column++)
            {
                ImGui::TableNextColumn();
                ImGui::Text("%f", tableau[row][column]);
            }
        }
        ImGui::EndTable();
    }
}

void GuiHandler::ConsoleInfo()
{
    std::cout << GetFPS() << std::endl;
}

void GuiHandler::OutPutToConsoleWindow()
{
    // ConsoleInfo();
    // Add a region to display the console output
    // ImGui::Text("Console Output:");

    ImGui::SetWindowFontScale(1.5f);
    ImGui::Separator();
    ImGui::TextUnformatted(imguiStreamBuffer.getConsoleBuffer().c_str());
    ImGui::Separator();
}

void GuiHandler::Draw()
{
    BeginDrawing();
    ClearBackground(DARKGRAY);

    // start ImGui Conent
    rlImGuiBegin();

    // ImGui Content fill the whole screen
    ImGui::SetNextWindowSize({static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())}, ImGuiCond_Always);
    ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Always);

    // ImGui Content stop resizing and remove title bar
    ImGui::Begin("Lp Solver Tool", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

    ImGui::Text("Welcome to Brett's LP Solver Tool");

    // static float floatValue = 0.0f;
    // static float floatValue2 = 0.0f;
    // ImGui::PushItemWidth(100);
    // ImGui::InputFloat("x1", &floatValue, 0.0f, 0.0f, "%.2f", ImGuiInputTextFlags_CharsDecimal);
    // ImGui::PopItemWidth();

    // ImGui::SameLine();

    // ImGui::PushItemWidth(100);
    // ImGui::InputFloat("x2", &floatValue2, 0.0f, 0.0f, "%.2f", ImGuiInputTextFlags_CharsDecimal);
    // ImGui::PopItemWidth();

    ImVec4 color(204.0f / 255.0f, 51.0f / 255.0f, 0.0f, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, color); // Set button color to red

    static std::vector<std::string> labels = {"x1", "x2"};

    static int counter = 2;
    // int counter{2};
    if (ImGui::Button("+"))
    {
        counter++;

        labels.resize(counter);                              // Resize the vector to accommodate the new element
        labels[counter - 1] = "x" + std::to_string(counter); // Access the element at counter-1
    }

    ImGui::SameLine();

    if (ImGui::Button("-"))
    {
        counter--;
    }
    ImGui::PopStyleColor();

    std::vector<float> floatValues(counter); // Assuming you want to store 2 float values

    ImGui::PushStyleColor(ImGuiCol_FrameBg, color); // Set button color to red

    ImGui::PushItemWidth(100);
    for (int i = 0; i < floatValues.size(); i++)
    {
        ImGui::InputFloat(labels[i].c_str(), &floatValues[i], 0.0f, 0.0f, "%.2f", ImGuiInputTextFlags_CharsDecimal);
        ImGui::SameLine();
    }
    ImGui::PopItemWidth();

    ImGui::Spacing();

    ImGui::PopStyleColor();

    OutPutToConsoleWindow();
    ImGui::Text("Table Output:");
    ImGui::Separator();
    SetUpTables();

    ImGui::End();

    // end ImGui Content
    rlImGuiEnd();

    EndDrawing();
}

/**
 * Helper function for the main loop.
 *
 * @param userData a pointer to user-defined data
 *
 * @return void
 *
 * @throws None
 */
void GuiHandler::MainLoopHelper(void *userData)
{
    GuiHandler *guiHandler = static_cast<GuiHandler *>(userData);

    // TODO init code here
    guiHandler->Draw();
}

/**
 * Runs the game loop.
 *
 * @throws ErrorType description of error
 */
void GuiHandler::Run()
{
#if defined(PLATFORM_WEB)
    emscripten_set_main_loop_arg(MainLoopHelper, this, 0, 1);
#else
    while (!WindowShouldClose())
    {
        MainLoopHelper(this);
    }
#endif
}