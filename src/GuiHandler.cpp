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

    // ImVec4 accentColor(204.0f / 255.0f, 51.0f / 255.0f, 0.0f, 1.0f);

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

void GuiHandler::HandelInput()
{
    // get the objective function

    ImGui::PushStyleColor(ImGuiCol_Button, accentColor); // Set button color to red

    // add and minus buttons for amount of objective functions
    if (ImGui::Button("+"))
    {

        objFunctionCtr++;
        objFunctionLabels.push_back("x" + std::to_string(objFunctionCtr));
        objFunction.push_back(0.0f);
        // amount of constraint variables is the amount of objective function variables
        for (int i = 0; i < static_cast<int>(constraints.size()); i++)
        {
            constraints[i].insert(constraints[i].begin(), 0.0f);
        }
        tempConstraints.insert(tempConstraints.end() - 1, 0.0f);
    }

    ImGui::SameLine();

    if (ImGui::Button("-"))
    {
        if (objFunctionCtr > 2)
        {
            objFunctionCtr--;
            objFunctionLabels.pop_back();
            objFunction.pop_back();
            for (int i = 0; i < static_cast<int>(constraints.size()); i++)
            {
                constraints[i].erase(constraints[i].begin(), constraints[i].begin() + 1);
            }
            tempConstraints.erase(tempConstraints.end() - 2);
        }
    }

    ImGui::PopStyleColor();
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(32.0f / 255.0f, 33.0f / 255.0f, 36.0f / 255.0f, 1.0f)); // Set button color to red
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, 50), true, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoBackground);
    ImGui::PushItemWidth(50);

    // get inputs for constraints
    for (int i = 0; i < static_cast<int>(objFunction.size()); i++)
    {
        ImGui::InputFloat(objFunctionLabels[i].c_str(), &objFunction[i], 0.0f, 0.0f, "%.2f", ImGuiInputTextFlags_CharsDecimal);
        ImGui::SameLine();
    }

    ImGui::PopItemWidth();
    ImGui::EndChild();
    ImGui::Spacing();
    ImGui::PopStyleColor();

    // get the constraints

    ImGui::PushStyleColor(ImGuiCol_Button, accentColor); // Set button color to red

    // add and minus buttons for amount of constraints
    std::string identifierAdd = std::string("##ButtonPlus") + std::to_string(constraintsCtr);
    if (ImGui::Button(("+" + identifierAdd).c_str()))
    {
        constraintsCtr++;
        constraints.push_back(tempConstraints);
    }

    ImGui::SameLine();

    std::string identifierMin = std::string("##ButtonMinus") + std::to_string(constraintsCtr);
    if (ImGui::Button(("-" + identifierMin).c_str()))
    {
        if (constraintsCtr != 1)
        {
            constraintsCtr--;
            constraints.pop_back();
        }
    }

    ImGui::PopStyleColor();
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(32.0f / 255.0f, 33.0f / 255.0f, 36.0f / 255.0f, 1.0f)); // Set button color to red
    ImGui::BeginChild("ScrollingRegion2", ImVec2(0, 35 * static_cast<int>(constraints.size())), true, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoBackground);
    ImGui::PushItemWidth(50);

    for (int i = 0; i < static_cast<int>(constraints.size()); i++)
    {
        int ctr{1};

        for (int j = 0; j < static_cast<int>(constraints[i].size() - 1); j++)
        {
            std::string identifier = std::string("##") + std::to_string(i) + std::to_string(j);
            ImGui::PushID(identifier.c_str());
            ImGui::InputFloat("", &constraints[i][j], 0.0f, 0.0f, "%.2f", ImGuiInputTextFlags_CharsDecimal);
            ImGui::PopID();
            ImGui::SameLine();

            if (ctr != static_cast<int>(objFunction.size() + 1))
            {
                ImGui::Text("X%d", ctr);
                ctr++;
                ImGui::SameLine();
            }

            if (j == static_cast<int>(objFunction.size() - 1))
            {
                ImGui::PushItemWidth(35);
                int index = constraints[i].back();

                // 0 or 1 at the end for <= or >= ... 0 being <= and 1 being >= 2 being =
                const char *signs[] = {"<=", ">="};
                const char *currentItem = signs[index];

                std::string id = std::string("##") + std::to_string(i);
                ImGui::PushID(id.c_str());
                ImGui::PushStyleColor(ImGuiCol_FrameBg, accentColor);
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, accentColor);
                ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(32.0f / 255.0f, 33.0f / 255.0f, 36.0f / 255.0f, 1.0f));

                if (ImGui::BeginCombo("", currentItem, ImGuiComboFlags_NoArrowButton))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(signs); n++)
                    {
                        bool is_selected = (currentItem == signs[n]);
                        if (ImGui::Selectable(signs[n], is_selected, ImGuiSelectableFlags_None, ImVec2(50, 25)))
                        {
                            constraints[i].back() = n;
                        }
                        if (is_selected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }

                    ImGui::EndCombo();
                }
                ImGui::PopID();

                ImGui::SameLine();
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
                ImGui::PopItemWidth();
            }
        }
        ImGui::NewLine();
    }

    ImGui::PopItemWidth();
    ImGui::EndChild();
    ImGui::Spacing();
    ImGui::PopStyleColor();

    // if (ImGui::Button("Solve"))
    // {
    // PassInputToSimplex();
    solve = true;
    // solve = false;
    // }

    // if (ImGui::Button("Solve"))
    // {
    //     // PassInputToSimplex();
    //     solve = true;
    //     // solve = false;
    // }
}

void GuiHandler::PassInputToSimplex()
{
    if (solve)
    {
        int ctrMax{};
        int ctrMin{};
        for (int i = 0; i < static_cast<int>(constraints.size()); i++)
        {
            if (constraints[i].back() == 0)
            {
                ctrMax++;
            }

            if (constraints[i].back() == 1)
            {
                ctrMin++;
            }
        }

        runOnce = solve;

        if (runOnce)
        {
            // runOnce = false;
            if (ctrMax == static_cast<int>(constraints.size()))
            {
                PrimalSolver *simplex = new PrimalSolver(objFunction, constraints);
                SetUpTables(simplex);
                delete simplex;
                runOnce = false;
            }
            else if (ctrMin == static_cast<int>(constraints.size()))
            {
                TwoPhase *simplex = new TwoPhase(objFunction, constraints);
                SetUpTables(simplex);
                delete simplex;
                runOnce = false;
            }
            else
            {
                TwoPhase *simplex = new TwoPhase(objFunction, constraints, true);
                SetUpTables(simplex);
                delete simplex;
                runOnce = false;
            }
        }
    }
}

void GuiHandler::SetUpTables(PrimalTwoPhaseBase *simplex)
{
    ImGui::Text("canonical form:");
    ImGui::Separator();
    ImGui::NewLine();
    for (const auto &row : simplex->GetCanonicalForm())
    {
        for (auto element : row)
        {
            // ImGui::Text("%s ", element);
            ImGui::Text("%s ", element.c_str());

            ImGui::SameLine();
        }
        ImGui::NewLine();
    }

    ImGui::NewLine();
    ImGui::Text("Solution:");
    ImGui::Separator();
    ImGui::NewLine();

    ImGui::Text("%s", simplex->GetSolution().c_str());

    ImGui::NewLine();
    ImGui::Text("Table Output:");
    ImGui::Separator();
    ImGui::NewLine();

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.0f, 10.0f));
    for (int i = 0; i < static_cast<int>(simplex->GetTableaus().size()); i++)
    {
        DisplayTable(simplex->GetTableaus(), i, simplex->GetPrimalSolveStep());
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
    }
    ImGui::PopStyleVar();
}

void GuiHandler::DisplayTable(std::vector<std::vector<std::vector<float>>> tab, int ctr, bool primalSolve)
{
    // add headings for tables
    std::vector<std::string> headings = {};

    for (int i = 0; i < static_cast<int>(objFunction.size()); i++)
    {
        headings.push_back("X" + std::to_string(i + 1));
    }

    int ctrOne{1};
    for (int i = 0; i < static_cast<int>(constraints.size()); i++)
    {
        if (constraints[i].back() == 1)
        {
            if (!primalSolve)
            {
                headings.push_back("A" + std::to_string(ctrOne));
            }

            headings.push_back("E" + std::to_string(ctrOne));
            ctrOne++;
        }
    }

    int ctrZero{1};
    for (int i = 0; i < static_cast<int>(constraints.size()); i++)
    {
        if (constraints[i].back() == 0)
        {
            headings.push_back("S" + std::to_string(ctrZero));
            ctrZero++;
        }
    }

    headings.push_back("RHS");
    headings.push_back("Theta");

    std::vector<std::vector<float>> tableau = tab[ctr];
    if (ImGui::BeginTable(("table" + std::to_string(ctr)).c_str(), static_cast<int>(tableau[0].size() + 1),
                          ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersOuterH |
                              ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersOuterV |
                              ImGuiTableFlags_SizingFixedFit |
                              ImGuiTableFlags_NoKeepColumnsVisible | ImGuiTableFlags_ScrollX,
                          ImVec2(0, ((18 * (ctr + 1) * 2) * static_cast<int>(tableau[0].size())))))
    {
        ImGui::TableNextRow();
        for (int column = 0; column < static_cast<int>(headings.size()); column++)
        {
            ImGui::TableNextColumn();
            ImGui::Text("%s", headings[column].c_str());
        }

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
    ImGui::NewLine();
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
    ImGui::Separator();

    // ImVec4 color(204.0f / 255.0f, 51.0f / 255.0f, 0.0f, 1.0f);
    HandelInput();
    // OutPutToConsoleWindow();
    ImGui::SetWindowFontScale(1.5f);

    // OutPutToConsoleWindow();
    // ImGui::Text("Table Output:");
    // ImGui::Separator();
    PassInputToSimplex();
    // SetUpTables();

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