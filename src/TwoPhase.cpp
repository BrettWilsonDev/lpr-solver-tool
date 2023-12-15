#include "TwoPhase.hpp"

TwoPhase::TwoPhase()
{
    Init();
}

TwoPhase::~TwoPhase()
{
}

void TwoPhase::Init()
{
    // TODO add = case and fixed round up or down error

    objFunction = {100, 30};

    // 0 or 1 at the end for <= or >= ... 0 being <= and 1 being >= 2 being =
    constraints = {
        {0, 1, 3, 1},
        {1, 1, 7, 0},
        {10, 4, 40, 0},
    };

    twoPhaseMax = true;

    // // objFunction = {100, 30};

    // // constraints = {
    // //     {2, 3, 4, 1},
    // //     {5, -6, 7, 1},
    // //     {0, 7, -8, 2},
    // // };

    // // objFunction = {100, 30, 20};

    // // constraints = {
    // //     {2, 3, 4, 4, 1},
    // //     {5, -6, 7, 4, 2},
    // //     {5, -6, 7, 4, 1},
    // //     {0, 7, -8, 4, 2},
    // // };

    // objFunction = {4, 5};

    // // 0 or 1 at the end for <= or >= ... 0 being <= and 1 being >= 2 being =
    // constraints = {
    //     {2, 3, 6, 0},
    //     {1, 1, 7, 1},
    // };

    // objFunction = {50, 100};

    // constraints = {
    //     {7, 2, 28, 1},
    //     {2, 12, 24, 1},
    // };

    // objFunction = {4, 1};

    // // 0 or 1 at the end for <= or >= ... 0 being <= and 1 being >= 2 being =
    // constraints = {
    //     {3, 1, 10, 1},
    //     {1, 1, 5, 1},
    //     {1, 0, 3, 1},
    // };

    // objFunction = {7, 1};

    // constraints = {
    //     {2, 1, 12, 1},
    //     {2, 3, 18, 1},
    // };

    // objFunction = {2, 3, 4};

    // constraints = {
    //     {3, 2, 1, 10, 0},
    //     {2, 3, 3, 15, 0},
    //     {1, 1, -1, 4, 1},
    // };

    objFunction = {3, 2, 3};

    constraints = {
        {2, 1, 1, 2, 0},
        {3, 4, 2, 8, 1},
    };

    // StandardForm();
    // BuildTableauMathForm();
    // Solve();
    // PrepSolutionDisplay();

    PrimalTwoPhaseBase::Init();
}

void TwoPhase::standardFormExtended()
{
    std::vector<std::vector<std::string>> temp;

    // remove the rows from the string with the incorrect max or min constraint
    for (int i = static_cast<int>(constraints.size()) - 1; i >= 0; i--)
    {
        // TODO handle min
        if (constraints[i].back() == 1 || constraints[i].back() == 2)
        {
            temp.push_back(canonical[i + 1]);
            canonical.erase(canonical.begin() + i + 1);
        }
    }

    // replace the slack vars with e
    for (int i = 0; i < static_cast<int>(temp.size()); i++)
    {
        for (int j = 0; j < static_cast<int>(temp[i].size()); j++)
        {
            for (int k = 0; k < static_cast<int>(constraints.size() + 1); k++)
            {
                if (temp[i][j] == "s" + std::to_string(k))
                {
                    temp[i][j] = "e";
                }
            }
        }
    }

    // add numbers to the artificial variables
    int ctr{1};
    for (auto &row : temp)
    {
        for (auto &element : row)
        {
            if (element == "e")
            {
                element = "a" + std::to_string(ctr) + " - e" + std::to_string(ctr);
                ctr++;
            }
        }
    }

    // add the two phase standard form rows
    for (int i = 0; i < static_cast<int>(temp.size()); i++)
    {
        canonical.insert(canonical.begin() + i + 1, temp[i]);
    }

    // add the objective w function
    std::vector<std::string> tempObj;
    if (temp.size() == 1)
    {
        // single row
        for (auto &element : temp[0])
        {
            if (element.find('a') != std::string::npos)
            {
                // incase of a large constraint amount
                if (constraints.size() < 9)
                {
                    element.replace(0, 5, "");
                }
                else
                {
                    element.replace(0, 4, "");
                }
            }

            if (element.find('+') != std::string::npos)
            {
                element = '-';
            }
        }

        temp[0][0].insert(0, "w + ");

        canonical.insert(canonical.begin(), temp[0]);
    }
    else
    {
        // multiple rows
        std::vector<std::vector<float>> tempCon;
        tempCon.reserve(temp.size());

        for (int i = 0; i < static_cast<int>(temp.size()); i++)
        {
            for (int j = 0; j < static_cast<int>(temp[i].size()); j++)
            {
                if (temp[i][j].find('a') != std::string::npos)
                {
                    // std::cout << constraints[i].back() << std::endl;
                    tempCon.push_back(constraints[i]);

                    float tempNum{};
                    // handle = case and build the w function rows to added to each other
                    if (constraints[i].back() == 2)
                    {
                        tempNum = constraints[i][constraints[i].size() - 2];
                        tempCon.back().pop_back();
                        tempCon.back().pop_back();
                        tempCon.back().push_back(0);
                        tempCon.back().push_back(tempNum);
                    }
                    else
                    {
                        tempNum = constraints[i][constraints[i].size() - 2];
                        tempCon.back().pop_back();
                        tempCon.back().pop_back();
                        tempCon.back().push_back(-1);
                        tempCon.back().push_back(tempNum);
                    }
                }
            }
        }

        // make an empty row of zeros to avoid index out of bounds errors :: not the best of solutions
        tempCon.push_back(std::vector<float>());
        for (int i = 0; i < static_cast<int>(tempCon[0].size()); i++)
        {
            tempCon.back().push_back(0);
        }

        for (int i = 0; i < static_cast<int>(tempCon.size() - 1); i++)
        {
            for (int j = 0; j < static_cast<int>(tempCon[i].size() - 1); j++)
            {
                tempCon[i][j] = -tempCon[i][j];
            }
        }

        // do the arithmetic for the w function
        for (int j = 0; j < static_cast<int>(tempCon[0].size()); j++)
        {
            float sum{};
            for (int i = 0; i < static_cast<int>(tempCon.size()); i++)
            {
                sum += tempCon[i][j];
            }
            tempObj.push_back(std::to_string(sum));
            // std::cout << "Sum of column " << j << ": " << sum << std::endl;
        }

        // used before stof
        std::string tempEql{tempObj.back()};
        int len{static_cast<int>(std::stof(tempObj.at(tempObj.size() - 2)))};
        tempObj.erase(tempObj.end() - 2);
        tempObj.pop_back();

        // changes back to the correct sign
        for (int i = 0; i < static_cast<int>(tempObj.size()); i++)
        {
            float floatValue = std::stof(tempObj[i]);
            floatValue = -floatValue;
            tempObj[i] = std::to_string(floatValue);
        }

        // add the signs as strings
        for (int i = 0; i < static_cast<int>(objFunction.size()); i++)
        {
            tempObj.insert(tempObj.begin() + 1 + i * 2, "+");
        }

        tempObj.pop_back();

        // add the e vars and their nums
        for (int i = 0; i < len; i++)
        {
            tempObj.push_back("- e" + std::to_string(i + 1));
        }

        tempObj.push_back(tempEql);

        tempObj.insert(tempObj.begin(), "w +");
        tempObj.insert(tempObj.end() - 1, "=");

        canonical.insert(canonical.begin(), tempObj);
    }
}

std::vector<std::vector<float>> TwoPhase::BuildTableauMathFormExtended()
{
    std::vector<std::vector<float>> tempTableau = tableauMathForm;
    std::vector<std::vector<float>> tempCon;

    // init tempCon
    int amtSlacks{};
    for (int i = 0; i < static_cast<int>(constraints.size()); i++)
    {
        if (constraints[i].back() == 1)
        {
            tempCon.push_back(constraints[i]);
        }

        if (constraints[i].back() == 0)
        {
            amtSlacks++;
        }
    }

    // zero out the correct amount of artificial spots
    for (int i = 0; i < static_cast<int>(tempCon.size()); i++)
    {
        float tempNum{};
        tempNum = tempCon[i][tempCon[i].size() - 2];
        tempCon[i].insert(tempCon[i].begin(), tempNum);
        tempCon[i].pop_back();
        tempCon[i].pop_back();

        for (int j = 0; j < static_cast<int>(tempCon.size()); j++)
        {
            tempCon[i].push_back(0);
            tempCon[i].push_back(0);
        }
    }

    // put -1 and 1 in correct spots : a1 e1 etc
    int ctr{};
    for (int i = 0; i < static_cast<int>(tempCon.size()); i++)
    {
        for (int j = static_cast<int>(objFunction.size() + 1 + ctr); j < static_cast<int>(tempCon[i].size()); j += 2)
        {
            if (j + 1 == static_cast<int>(tempCon[i].size()))
            {
                break;
            }

            tempCon[i][j] = 1;
            tempCon[i][j + 1] = -1;
            break;
        }
        ctr += 2;
    }

    // add correct amount of 0s from slack vars
    for (int i = 0; i < static_cast<int>(tempCon.size()); i++)
    {
        for (int j = 0; j < amtSlacks; j++)
        {
            tempCon[i].push_back(0);
        }

        tempTableau[0].push_back(0);

        // put the sum in the correct spot
        tempCon[i].push_back(tempCon[i].front());
        tempCon[i].erase(tempCon[i].begin());
    }

    // do the arithmetic for the w function
    std::vector<float> tempObj;
    for (int j = 0; j < static_cast<int>(tempCon[0].size()); j++)
    {
        float sum{};
        for (int i = 0; i < static_cast<int>(tempCon.size()); i++)
        {
            sum += tempCon[i][j];
        }
        tempObj.push_back(sum);
    }

    // remove artificial variables from the w function
    for (int i = static_cast<int>(objFunction.size()); i < static_cast<int>(tempObj.size() - 1); i++)
    {
        if (tempObj[i] == 1)
        {
            tempObj[i] = 0;
        }
    }

    // put the w function in the table
    tempTableau.insert(tempTableau.begin(), tempObj);

    // add tempCon and leading zeros to the table
    int len = static_cast<int>(tempCon.size());
    for (int i = 0; i < static_cast<int>(constraints.size()); i++)
    {
        if (constraints[i].back() == 0)
        {
            for (int j = 0; j < len; j++)
            {
                tempTableau[i + 2].insert(tempTableau[i + 2].begin() + static_cast<int>(objFunction.size()), 0);
            }
        }
        if (constraints[i].back() == 1)
        {
            tempTableau[i + 2] = tempCon.front();
            tempCon.erase(tempCon.begin());
        }
    }

    for (int i = 0; i < static_cast<int>(tempTableau[1].size() - 1); i++)
    {
        if (tempTableau[1][i] != 0)
        {
            tempTableau[1][i] = -tempTableau[1][i];
        }
    }

    return tempTableau;
}

void TwoPhase::PerformPivotOperationsExtended(std::vector<std::vector<float>> tab)
{
    if (tab[1].back() != 0)
    {
        // stop theta column from stacking
        for (int i = 2; i < static_cast<int>(tab.size()); i++)
        {
            tab[i].pop_back();
        }
    }

    std::vector<std::vector<float>> tableauStageOne = tab;
    std::vector<float> objFunctionMath = tab[0];

    // remove the sum form being included
    if (objFunctionMath.back() != 0)
    {
        objFunctionMath.pop_back();
    }

    float pivotElement{};
    if (maxObj)
    {
        pivotElement = -*std::min_element(objFunctionMath.rbegin(), objFunctionMath.rend());
    }
    else
    {
        pivotElement = *std::max_element(objFunctionMath.rbegin(), objFunctionMath.rend());
    }

    // dont forget the sign check
    auto pivotColumnIterator = std::find_if(objFunctionMath.begin(), objFunctionMath.end(), [pivotElement](float value)
                                            { return std::abs(value) == std::abs(pivotElement); });
    int pivotColumn = std::distance(objFunctionMath.begin(), pivotColumnIterator);

    // std::cout << "pivotElement: " << pivotElement << std::endl;
    // std::cout << "pivot column: " << pivotColumn << std::endl;

    // calculate percentages in theta column
    for (int i = 2; i < static_cast<int>(tableauStageOne.size()); i++)
    {
        tableauStageOne[i].push_back(tableauStageOne[i].back() / tableauStageOne[i][pivotColumn]);
    }

    // create a temporay vector to hold theta column and items
    std::vector<std::vector<float>> tempTab = tableauStageOne;

    // remove any negative from the theta column test by making them infinity
    for (int i = 0; i < static_cast<int>(tempTab.size()); i++)
    {
        if (tempTab[i].back() < 0)
        {
            tempTab[i].back() = std::numeric_limits<float>::infinity();
        }
    }

    // find smallest theta value and set pivot row
    float smallest = tempTab[2].back();
    int pivotRow{2};
    for (int i = 3; i < static_cast<int>(tempTab.size()); i++)
    {
        float current = tempTab[i].back();
        if (current < smallest)
        {
            smallest = current;
            pivotRow = i;
        }
    }

    std::vector<std::vector<float>> theta = tempTab;

    // std::cout << "pivot element: " << smallest << std::endl;

    // std::cout << "pivot row: " << pivotRow << std::endl;

    // fix theta back its correct values in place of infinity
    for (int i = 1; i < static_cast<int>(tempTab.size()); i++)
    {
        tempTab[i].pop_back();
        tempTab[i].push_back(tableauStageOne[i].back());
    }

    // move on to new tableau
    std::vector<std::vector<float>> tableauStageTwo = tableauStageOne;

    // remove theta column
    for (int i = 2; i < static_cast<int>(tableauStageTwo.size()); i++)
    {
        tableauStageTwo[i].pop_back();
        // tableauStageOne[i].pop_back();
    }

    // do pivot row operation step
    for (int j = 0; j < static_cast<int>(tableauStageTwo[pivotRow].size()); j++)
    {
        tableauStageTwo[pivotRow][j] = tableauStageOne[pivotRow][j] / tableauStageOne[pivotRow][pivotColumn];
    }

    // pivot operation for new tableau primal simplex
    for (int i = 0; i < static_cast<int>(tableauMathForm.size()); i++)
    {
        for (int j = 0; j < static_cast<int>(tableauMathForm[i].size()); j++)
        {
            if (i == pivotRow)
            {
                continue;
            }
            if (i < static_cast<int>(tableauStageTwo.size()) && j < static_cast<int>(tableauStageTwo[i].size()))
            {
                // the formula: Element_New_Table((i, j)) = Element_Old_Table((i, j)) - (Element_Old_Table((i, Pivot_column)) * Element_New_Table((Pivot_Row, j)))
                tableauStageTwo[i][j] = ((tableauStageOne[i][j]) - (tableauStageOne[i][pivotColumn] * tableauStageTwo[pivotRow][j]));
            }
            // std::cout << std::endl;
        }
        // std::cout << std::endl;
    }

    // //add theta column back
    for (int i = 2; i < static_cast<int>(tableauStageTwo.size()); i++)
    {
        tableauStageTwo[i].push_back(tempTab[i].back());
    }

    for (auto &row : tableauStageTwo)
    {
        for (auto &item : row)
        {
            if (std::abs(item) < 1e-6)
            {
                item = 0.0;
            }
        }
    }

    tableau = tableauStageTwo;
    // tableausExtended.push_back(tableauStageTwo);
    tableaus.push_back(tableauStageTwo);
}

void TwoPhase::SolveExtended()
{
    // check if the problem is solved by seeing if the objective function is all positive or all negative for max and min problems
    int ctr{};
    bool allPosOrNeg{};
    bool isSolved{};
    // while (!isSolved && ctr < 100)
    while (!isSolved && ctr < 100)
    {

        allPosOrNeg = std::all_of(tableau[0].begin(), tableau[0].end() - 1, [](int num)
                                  { return num <= 0; });

        // if (allPosOrNeg && tableau[0].back() == 0)
        if (allPosOrNeg && tableau[0].back() == 0)
        {
            isSolved = true;
            // std::cout << "the solution is: bob" << tableau[1].back() << std::endl;
            break;
        }

        ctr++;
        PerformPivotOperationsExtended(tableau);

        for (auto &row : tableau)
        {
            for (auto &item : row)
            {
                int decimalPlaces = 7;
                item = std::round(item * std::pow(10, decimalPlaces)) / std::pow(10, decimalPlaces);
                // item = std::round(item);
            }
        }
        // std::cout << (allPosOrNeg && tableau[0].back() == 0) << std::endl;
        // std::cout << tableau[0].back() << std::endl;
    }

    tableau.erase(tableau.begin());

    int amtAVar{0};
    for (int i = 0; i < static_cast<int>(constraints.size()); i++)
    {
        if (constraints[i].back() == 1)
        {
            amtAVar++;
        }
    }

    int amtBVar{0};
    for (int i = 0; i < amtAVar; i++)
    {
        for (int j = 0; j < static_cast<int>(tableau.size()); j++)
        {
            // std::cout << *(tableau[j].begin() + 1) << std::endl;
            tableau[j].erase(tableau[j].begin() + 2 + amtBVar);
        }
        amtBVar++;
    }

    tableaus.push_back(tableau);

    maxObj = twoPhaseMax;

    // // TODO move this output to imgui
    if (ctr == 100)
    {
        std::cout << "unbounded" << std::endl;
    }
}

void TwoPhase::PrepSolutionDisplayExtended()
{
    // Dont forget its a e not e a or e e e aaa

    // move theta to the correct table for display
    // for (int i = 2; i < static_cast<int>(tableausExtended.size()); i++)
    // {
    //     for (int j = 1; j < static_cast<int>(tableausExtended[i].size()); j++)
    //     {
    //         tableausExtended[i - 2][j].back() = tableausExtended[i][j].back();
    //     }
    // }

    // // zero out theta column in last table
    // // for (int j = 2; j < static_cast<int>(tableaus[0].size()); j++)
    // // {
    // //     tableausExtended.back()[j].back() = 0;
    // // }

    // std::cout << std::endl;
}