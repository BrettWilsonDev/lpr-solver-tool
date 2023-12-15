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
}

void GuiHandler::Unload()
{
    rlImGuiShutdown();

    std::cout.rdbuf(coutBuffer);

    CloseWindow();
}

void GuiHandler::SetUpTables()
{
    // ImGui::BeginChild("ScrollingRegion", ImVec2(0, 200), true, ImGuiWindowFlags_HorizontalScrollbar);
    // ImGui::Begin("Scrolling Region", nullptr, ImGuiWindowFlags_HorizontalScrollbar);

    // float regionWidth = ImGui::GetContentRegionAvail().x;
    // // float regionWidth = 200;
    // // float regionHeight = FLT_MAX;
    // float regionHeight = 200;

    // ImGui::BeginChild("ScrollingRegion", ImVec2(1000, 200), true, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar);
    // ImGui::BeginChild("ScrollingRegion", ImVec2(1000, 0), true, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);

    // ImGui::InvisibleButton("InvisibleElement", ImVec2(10000, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.0f, 10.0f));
    // ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(1.0f, 1.0f));
    // ImGui::SetWindowFontScale(1.0f);
    for (int i = 0; i < static_cast<int>(twoPhase.GetTableaus().size()); i++)
    {
        DisplayTable(twoPhase.GetTableaus(), i);
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
    }
    // ImGui::PopStyleVar();
    // ImGui::EndChild();
}

void GuiHandler::DisplayTable(std::vector<std::vector<std::vector<float>>> tab, int ctr)
{
    std::vector<std::vector<float>> tableau = tab[ctr];

    // if (ImGui::BeginTable("table1", static_cast<int>(tableau[0].size() + 1), ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersOuterV /*| ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedFit */))
    // if (ImGui::BeginTable("table1", static_cast<int>(tableau[0].size() + 1)))
    if (ImGui::BeginTable("table1", static_cast<int>(tableau[0].size() + 1),
                          ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersOuterH |
                              ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersOuterV |
                              ImGuiTableFlags_SizingFixedFit |
                              ImGuiTableFlags_NoKeepColumnsVisible | ImGuiTableFlags_ScrollX, ImVec2(0, (30 * static_cast<int>(tableau[0].size())))))
    {

        ImGui::TableSetupColumn("Header 1", ImGuiTableColumnFlags_WidthFixed, 200.0f);
        // ImGui::SetColumnWidth(1, 200);
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

    // if (ImGui::BeginTable("table1", static_cast<int>(tableau[0].size() + 1),
    //                       ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY))
    // {
    //     // Set column widths or sizing flags as needed
    //     ImGui::TableSetupColumn("Header 1", ImGuiTableColumnFlags_WidthFixed, 200.0f);
    //     ImGui::TableSetupColumn("Header 2", ImGuiTableColumnFlags_WidthFixed, 200.0f);
    //     ImGui::TableSetupColumn("Header 3", ImGuiTableColumnFlags_WidthFixed, 200.0f);

    //     ImGui::TableHeadersRow();

    //     for (int row = 0; row < static_cast<int>(tableau.size()); row++)
    //     {
    //         ImGui::TableNextRow(1, 1000.0f );

    //         for (int column = 0; column < static_cast<int>(tableau[row].size()); column++)
    //         {
    //             ImGui::TableNextColumn();
    //             ImGui::Text("%f", tableau[row][column]);
    //         }
    //     }

    //     ImGui::EndTable();
    // }
}

void GuiHandler::ConsoleInfo()
{
    std::cout << GetFPS() << std::endl;
}

void GuiHandler::OutPutToConsoleWindow()
{
    // ConsoleInfo();
    // Add a region to display the console output
    ImGui::Text("Console Output:");

    ImGui::SetWindowFontScale(1.5f);
    ImGui::Separator();
    ImGui::TextUnformatted(imguiStreamBuffer.getConsoleBuffer().c_str());
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
    ImGui::Begin("Lpr Tool", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

    ImGui::Text("Table Output:");
    OutPutToConsoleWindow();
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