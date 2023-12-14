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
    objFunction = {100, 30};

    // 0 or 1 at the end for <= or >= ... 0 being <= and 1 being >= 2 being =
    constraints = {
        {0, 1, 3, 1},
        {1, 1, 7, 0},
        {10, 4, 40, 0},
    };

    // objFunction = {100, 30};

    // constraints = {
    //     {2, 3, 4, 1},
    //     {5, -6, 7, 1},
    //     {0, 7, -8, 2},
    // };

    // objFunction = {100, 30, 20};

    // constraints = {
    //     {2, 3, 4, 4, 1},
    //     {5, -6, 7, 4, 2},
    //     {5, -6, 7, 4, 1},
    //     {0, 7, -8, 4, 2},
    // };

    // objFunction = {4, 5};

    // // 0 or 1 at the end for <= or >= ... 0 being <= and 1 being >= 2 being =
    // constraints = {
    //     {2, 3, 6, 0},
    //     {1, 1, 7, 1},
    // };

    objFunction = {50, 100};

    constraints = {
        {7, 2, 28, 1},
        {2, 12, 24, 1},
    };

    // objFunction = {4, 1};

    // // 0 or 1 at the end for <= or >= ... 0 being <= and 1 being >= 2 being =
    // constraints = {
    //     {3, 1, 10, 1},
    //     {1, 1, 5, 1},
    //     {1, 0, 3, 1},
    // };

    StandardForm();
    BuildTableauMathForm();
    // Solve();
    // PrepSolutionDisplay();

    PerformPivotOperationsExtended(tableau);

    PerformPivotOperationsExtended(tableau);
    // PerformPivotOperationsExtended(tableau);

    // PerformPivotOperationsExtended(tableau);


    // tableau.erase(tableau.begin());

    // int amtAVar{0};
    // for (int i = 0; i < static_cast<int>(constraints.size()); i++)
    // {
    //     if (constraints[i].back() == 1)
    //     {
    //         amtAVar++;
    //     }
    // }

    // int amtBVar{0};
    // for (int i = 0; i < amtAVar; i++)
    // {
    //     for (int j = 0; j < static_cast<int>(tableau.size()); j++)
    //     {
    //         // std::cout << *(tableau[j].begin() + 1) << std::endl;
    //         tableau[j].erase(tableau[j].begin() + 2 + amtBVar);
    //     }
    //     amtBVar++;
    // }

    // PerformPivotOperations(tableau);

    // // PerformPivotOperations(tableau);

    // for (const auto &table : tableaus)
    // {
    //     for (const auto &row : table)
    //     {
    //         for (auto element : row)
    //         {
    //             std::cout << element << " ";
    //         }
    //         std::cout << std::endl;
    //     }
    //     std::cout << std::endl;
    // }
}

void TwoPhase::StandardForm()
{
    // build canonical form for display

    // temp string vector for building canonical form
    std::vector<std::string> tempVec = {};
    // convert objective function to string vector
    for (int i = 0; i < static_cast<int>(objFunction.size()); i++)
    {
        tempVec.push_back(std::to_string(objFunction[i]));
        if (i != static_cast<int>(objFunction.size()) - 1)
        {
            tempVec.push_back("-");
        }
    }
    canonical.push_back(tempVec);
    tempVec.clear();

    // convert constraints to string vector
    for (const auto &row : constraints)
    {
        for (auto element : row)
        {
            tempVec.push_back(std::to_string(element));
        }
        canonical.push_back(tempVec);
        tempVec.clear();
    }

    // adds symbols to the objective function
    int l = 1;
    for (auto &item : canonical[0])
    {
        if (!(item == "-" || item == "+"))
        {
            item = item + "x" + std::to_string(l);
            l++;
        }
    }

    // turn the objective function into standard form
    canonical[0].insert(canonical[0].begin(), "z -");
    canonical[0].insert(canonical[0].end(), "= 0");

    for (int i = 1; i < static_cast<int>(canonical.size()); i++)
    {
        // remove the <= or >= indicators
        canonical[i].pop_back();
        // add slack vars to the constraints
        canonical[i].insert(canonical[i].end() - 1, "s" + std::to_string(i));
        // add = to last element for canonical
        canonical[i].insert(canonical[i].end() - 1, "=");
    }

    // add the decision variable strings to the constraints
    for (int i = 1; i < static_cast<int>(canonical.size()); i++)
    {
        int j = 1;
        for (auto &element : canonical[i])
        {
            // only add x1, x2, x3... to numbers not the slack vars and so on
            if (element == "s" + std::to_string(i))
            {
                break;
            }

            element = element + "x" + std::to_string(j);
            j++;
        }
    }

    // add + between each number
    bool isFirstRow = true;
    for (auto &row : canonical)
    {
        // the first row is not a constraint
        if (isFirstRow)
        {
            isFirstRow = false;
            continue;
        }

        // add + between each number skips two index cause we add an new index
        for (auto i = row.begin() + 1; i != row.end(); std::advance(i, 2))
        {
            // last index is a = not a +
            if (std::next(i) == row.end() - 1)
            {
                break;
            }
            i = row.insert(i, "+");
        }
    }

    // remove 0.0 and 1.0 from the canonical form
    for (auto &row : canonical)
    {
        for (auto &element : row)
        {
            if (element == "z -" || element == "-" || element == "+" || element == "=" || element == "= 0")
            {
                continue;
            }

            bool con{};
            for (int i = 1; i < static_cast<int>(constraints.size() + 1); i++)
            {
                if (element == "s" + std::to_string(i))
                {
                    con = true;
                    break;
                }
            }
            if (con)
            {
                continue;
            }

            try
            {
                // WASM dose not like stof
                float floatValue = std::stof(element);
                // float floatValue = std::atof(element.c_str());
                if (floatValue == 0.0f)
                {
                    element = "b";
                }
                if (floatValue == 1.0f)
                {
                    element.replace(element.find(std::to_string(1.0f)), std::to_string(1.0f).length(), "");
                }
            }
            catch (const std::invalid_argument &)
            {
                // Handle the case where `element` cannot be converted to a float
                std::cout << "Invalid argument: " << element << std::endl;
                continue;
            }
            catch (const std::out_of_range &)
            {
                // Handle the case where the converted float value is out of range
                std::cout << "Out of range: " << element << std::endl;
                continue;
            }
        }
    }

    // remove double plus signs
    for (auto &row : canonical)
    {
        for (auto i = row.begin(); i != row.end(); ++i)
        {
            auto &element = *i;        // Current element
            auto nextI = std::next(i); // Iterator pointing to the next element

            if (nextI != row.end())
            {
                auto &nextElement = *nextI;

                if (element == "b" && nextElement == "+")
                {
                    nextElement = "b";
                }
            }
            else
            {
                continue;
            }
        }
    }

    // remove extra space b for blank
    for (auto &row : canonical)
    {
        for (auto &element : row)
        {
            if (element == "b")
            {
                row.erase(std::remove(row.begin(), row.end(), "b"), row.end());
            }
        }
    }

    standardFormExtended();

    // TODO move display to imgui not through cout

    // display the canonical form temporay here until implemented in imgui
    std::cout << "canonical form:" << std::endl;
    for (const auto &row : canonical)
    {
        for (auto element : row)
        {
            std::cout << element << " ";
        }
        std::cout << std::endl;
    }
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

void TwoPhase::BuildTableauMathForm()
{
    // check if the problem is max or min
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

    if (ctrMax == static_cast<int>(constraints.size()))
    {
        maxObj = true;
    }
    else if (ctrMin == static_cast<int>(constraints.size()))
    {
        maxObj = false;
    }
    else
    {
        // TODO handle the case this not a primal simplex problem
        std::cout << "critical error" << std::endl;
        std::cout << "this is not a primal simplex problem and thus you should not be seeing this message" << std::endl;
    }

    // create new vectors keeping the original input untouched
    std::vector<std::vector<float>> constraintsTab = constraints;
    std::vector<float> objFunctionRow = objFunction;

    // remove max or min indicators
    for (auto &row : constraintsTab)
    {
        row.pop_back();
    }

    // add slack vars spots
    int lenOfObjFunction = objFunction.size();
    int lenOfConstraints = constraintsTab.size();
    // std::cout << (lenOfObjFunction) << std::endl;
    for (int i = 0; i < static_cast<int>(constraintsTab.size()); i++)
    {
        for (int j = 0; j < lenOfConstraints; j++)
        {
            constraintsTab[i].insert(constraintsTab[i].begin() + lenOfObjFunction, 0.0f);
        }
    }

    // fill in slack vars in thier correct spots
    for (int i = 0; i < static_cast<int>(constraintsTab.size()); i++)
    {
        // it just so happens i is the same amount of slack vars ex: 3 rows 3 slack vars
        constraintsTab[i][i + lenOfObjFunction] = 1;
    }

    // handle max or min var signs
    if (maxObj)
    {
        for (auto &item : objFunctionRow)
        {
            item = -item;
        }
    }
    else
    {
        for (auto &item : objFunctionRow)
        {
            item = abs(item);
        }
    }

    // fill in objective function with 0s
    for (int i = 0; i < static_cast<int>(constraintsTab.size() + 1); i++)
    {
        objFunctionRow.push_back(0.0f);
    }

    // combine the tables
    tableauMathForm = constraintsTab;
    tableauMathForm.insert(tableauMathForm.begin(), objFunctionRow);

    tableauMathForm = BuildTableauMathFormExtended();

    // Servers a test display
    // std::cout << "display simple:" << std::endl;
    // for (auto &row : tableauMathForm)
    // {
    //     for (auto &item : row)
    //     {
    //         std::cout << item << " ";
    //     }
    //     std::cout << std::endl;
    // }

    tableau = tableauMathForm;
    // tableausExtended.push_back(tableauMathForm);
    tableaus.push_back(tableauMathForm);
}

/**
 * Builds the tableau math form extended.
 *
 * @param None
 *
 * @return None
 *
 *  * @throws None
 */
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
        // std::cout << "Sum of column " << j << ": " << sum << std::endl;
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
            // i += 2;
            for (int j = 0; j < len; j++)
            {
                tempTableau[i + 2].insert(tempTableau[i + 2].begin() + static_cast<int>(objFunction.size()), 0);
            }

            // tempTableau[i].insert(tempTableau[i].begin() + static_cast<int>(objFunction.size()), 0);
            // i -= 2;
        }
        if (constraints[i].back() == 1)
        {
            tempTableau[i + 2] = tempCon.front();
            tempCon.erase(tempCon.begin());
        }
    }

    // for (int i = 2; i < static_cast<int>(tempTableau.size()); i++)
    // {
    //     tempTableau[i].push_back(0);
    // }

    for (int i = 0; i < static_cast<int>(tempTableau[1].size() - 1); i++)
    {
        if (tempTableau[1][i] != 0)
        {
            tempTableau[1][i] = -tempTableau[1][i];
        }
    }

    std::cout << "display extended:" << std::endl;
    for (auto &row : tempTableau)
    {
        for (auto &item : row)
        {
            std::cout << item << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    return tempTableau;
}

/**
 * Performs pivot operations on a given tableau.
 *
 * @param tab the tableau to perform pivot operations on
 *
 * @return void
 *
 * @throws None
 */
void TwoPhase::PerformPivotOperations(std::vector<std::vector<float>> tab)
{

    // for (auto &row : tab)
    // {
    //     for (auto &item : row)
    //     {
    //         // item = -item;
    //         std::cout << item << " ";
    //     }
    //     std::cout << std::endl;
    // }
    // std::cout << std::endl;

    // std::cout << "\nperforming steps" << std::endl;

    // make sure it's not the initial mathematical form
    if (tab[0].back() != 0)
    {
        // stop theta column from stacking
        for (int i = 1; i < static_cast<int>(tab.size()); i++)
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

    // for (auto &item : objFunctionMath)
    // {
    //     std::cout << item << " ";
    // }
    // std::cout << std::endl;

    // // select column based on largest value
    // // int max = std::max(*std::max_element(objFunctionMath.begin(), objFunctionMath.end()), -*std::min_element(objFunctionMath.begin(), objFunctionMath.end()));
    // // int max = -*std::min_element(objFunctionMath.rbegin(), objFunctionMath.rend());

    int pivotElement{};
    if (maxObj)
    {
        pivotElement = -*std::min_element(objFunctionMath.rbegin(), objFunctionMath.rend());
    }
    else
    {
        pivotElement = *std::max_element(objFunctionMath.rbegin(), objFunctionMath.rend());
    }

    // dont forget the sign check
    auto pivotColumnIterator = std::find_if(objFunctionMath.begin(), objFunctionMath.end(), [pivotElement](int value)
                                            { return std::abs(value) == std::abs(pivotElement); });
    int pivotColumn = std::distance(objFunctionMath.begin(), pivotColumnIterator);

    // std::cout << "max: " << pivotElement << std::endl;
    // std::cout << "pivot column: " << pivotColumn << std::endl;

    // calculate percentages in theta column
    for (int i = 1; i < static_cast<int>(tableauStageOne.size()); i++)
    {
        tableauStageOne[i].push_back(tableauStageOne[i].back() / tableauStageOne[i][pivotColumn]);
    }

    // // for (auto &row : tableauStageOne)
    // // {
    // //     for (auto &item : row)
    // //     {
    // //         std::cout << item << " ";
    // //     }
    // //     std::cout << std::endl;
    // // }

    // create a temporay vector to hold theta column and items
    std::vector<std::vector<float>> tempTab = tableauStageOne;

    // remove any negative from the theta column test by making them infinity
    for (int i = 0; i < static_cast<int>(tempTab.size()); i++)
    {
        if (tempTab[i].back() < 0)
        {
            tempTab[1].back() = std::numeric_limits<float>::infinity();
        }
    }

    // find smallest theta value and set pivot row
    float smallest = tempTab[1].back();
    int pivotRow{1};
    for (int i = 2; i < static_cast<int>(tempTab.size()); i++)
    {
        float current = tempTab[i].back();
        if (current < smallest)
        {
            smallest = current;
            pivotRow = i;
        }
    }

    std::vector<std::vector<float>> theta = tempTab;

    // std::cout << "pivot row: " << pivotRow << std::endl;

    // fix theta back its correct values in place of infinity
    for (int i = 1; i < static_cast<int>(tempTab.size()); i++)
    {
        tempTab[i].pop_back();
        tempTab[i].push_back(tableauStageOne[i].back());
    }

    // std::cout << "pivot row: " << pivotRow << std::endl;

    // move on to new tableau
    std::vector<std::vector<float>> tableauStageTwo = tableauStageOne;

    // remove theta column
    for (int i = 1; i < static_cast<int>(tableauStageTwo.size()); i++)
    {
        tableauStageTwo[i].pop_back();
        // tableauStageOne[i].pop_back();
    }

    // do pivot row operation step
    for (int j = 0; j < static_cast<int>(tableauStageTwo[pivotRow].size()); j++)
    {
        tableauStageTwo[pivotRow][j] = tableauStageOne[pivotRow][j] / tableauStageOne[pivotRow][pivotColumn];
    }

    // for (auto &row : tableauStageTwo)
    // {
    //     for (auto &item : row)
    //     {
    //         std::cout << item << " ";
    //     }
    //     std::cout << std::endl;
    // }

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
        }
    }

    // add theta column back
    for (int i = 1; i < static_cast<int>(tableauStageTwo.size()); i++)
    {
        tableauStageTwo[i].push_back(tempTab[i].back());
    }

    // std::cout << "\ndisplay math:" << std::endl;

    // for (auto &row : tableauStageTwo)
    // {
    //     for (auto &item : row)
    //     {
    //         std::cout << item << " ";
    //     }
    //     std::cout << std::endl;
    // }

    tableau = tableauStageTwo;
    tableaus.push_back(tableauStageTwo);
}

void TwoPhase::PerformPivotOperationsExtended(std::vector<std::vector<float>> tab)
{
    // for (auto const row : tab)
    // {
    //     for (auto const item : row)
    //     {
    //         std::cout << item << " ";
    //     }
    //     std::cout << std::endl;
    // }
    // std::cout << std::endl;

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
        // pivotElement = *std::max_element(objFunctionMath.rbegin(), objFunctionMath.rend());

        // auto minElement = *std::min_element(objFunctionMath.rbegin(), objFunctionMath.rend());
        // if (minElement != 0)
        // {
        //     pivotElement = -minElement;
        // }
    }
    else
    {
        pivotElement = *std::max_element(objFunctionMath.rbegin(), objFunctionMath.rend());
        // auto minElement = *std::min_element(objFunctionMath.rbegin(), objFunctionMath.rend());
        // if (minElement != 0)
        // {
        //     pivotElement = -minElement;
        // }
    }

    // dont forget the sign check
    auto pivotColumnIterator = std::find_if(objFunctionMath.begin(), objFunctionMath.end(), [pivotElement](float value)
                                            { return std::abs(value) == std::abs(pivotElement); });
    int pivotColumn = std::distance(objFunctionMath.begin(), pivotColumnIterator);

    std::cout << "pivotElement: " << pivotElement << std::endl;
    std::cout << "pivot column: " << pivotColumn << std::endl;

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

    std::cout << "pivot element: " << smallest << std::endl;

    std::cout << "pivot row: " << pivotRow << std::endl;

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
                // std::cout << tableauStageTwo[i][j] << ": " << tableauStageOne[i][j] << " - " << tableauStageOne[i][pivotColumn] << " * " << tableauStageTwo[pivotRow][j] << " ";
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

    for (auto const row : tableauStageTwo)
    {
        for (auto const item : row)
        {
            std::cout << item << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

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

void TwoPhase::Solve()
{
    SolveExtended();

    // check if the problem is solved by seeing if the objective function is all positive or all negative for max and min problems
    int ctr{};
    bool allPosOrNeg{};
    bool isSolved{};
    while (!isSolved && ctr < 100)
    {
        PerformPivotOperations(tableau);

        if (maxObj)
        {
            // allPosOrNeg = std::all_of(tableau[0].begin(), tableau[0].end() - 1, [](int num)
            //                           { return num >= 0; });
            allPosOrNeg = std::all_of(tableau[0].begin(), tableau[0].end() - 1, [](int num)
                                      { return num <= 0; });
        }
        // else
        // {
        //     allPosOrNeg = std::all_of(tableau[0].begin(), tableau[0].end() - 1, [](int num)
        //                               { return num <= 0; });
        // }

        if (allPosOrNeg)
        {
            isSolved = true;
        }

        ctr++;
    }

    // TODO move this output to imgui
    if (ctr == 100)
    {
        std::cout << "unbounded" << std::endl;
    }

    if (isSolved)
    {
        std::cout << "the solution is: " << tableau[0].back() << std::endl;
    }
}

void TwoPhase::SolveExtended()
{
    // int ctr{};
    // bool allPosOrNeg{};
    // bool isSolved{};

    // std::vector<std::vector<float>> tempTab = tableau;
    // std::cout << std::endl;
    // PerformPivotOperationsExtended(tableau);
    // tempTab = tableau;

    // for (const auto &row : tableau)
    // {
    //     for (auto element : row)
    //     {
    //         std::cout << element << " ";
    //     }
    //     std::cout << std::endl;
    // }
    // std::cout << std::endl;

    // PerformPivotOperationsExtended(tableau);

    // allPosOrNeg = std::all_of(tableau[0].begin(), tableau[0].end() - 1, [](int num)
    //                           { return num <= 0; });

    // // if (allPosOrNeg && tableau[0].back() == 0)
    // if (allPosOrNeg && tableau[0].back() == 0)
    // {
    //     isSolved = true;
    //     // PerformPivotOperations(tempTab);
    //     std::cout << "aiofiwuasyhufuiahfhuashyuifghyu" << std::endl;
    // }
    // else
    // {
    //     std::cout << allPosOrNeg << "  " << (tableau[0].back()) << std::endl;
    // }

    // PerformPivotOperationsExtended(tableau);
    // tableau[1].push_back(360);

    // for (const auto &row : tableau)
    // {
    //     for (auto element : row)
    //     {
    //         std::cout << element << " ";
    //     }
    //     std::cout << std::endl;
    // }
    // std::cout << std::endl;

    // check if the problem is solved by seeing if the objective function is all positive or all negative for max and min problems
    // int ctr{};
    // bool allPosOrNeg{};
    // bool isSolved{};
    // // while (!isSolved && ctr < 100)
    // while (!isSolved && ctr < 100)
    // {
    //     PerformPivotOperationsExtended(tableau);

    //     allPosOrNeg = std::all_of(tableau[0].begin(), tableau[0].end() - 1, [](int num)
    //                               { return num <= 0; });

    //     // if (allPosOrNeg && tableau[0].back() == 0)
    //     if (allPosOrNeg && tableau[0].back() == 0)
    //     {
    //         isSolved = true;
    //     }
    //     ctr++;
    //     std::cout << (allPosOrNeg && tableau[0].back() == 0) << std::endl;
    //     std::cout << tableau[0].back() << std::endl;
    // }

    // // std::cout << ctr << std::endl;

    std::vector<std::vector<float>> tempTab = tableau;

    // check if the problem is solved by seeing if the objective function is all positive or all negative for max and min problems
    int ctr{};
    bool allPosOrNeg{};
    bool isSolved{};
    // while (!isSolved && ctr < 100)
    while (!isSolved && ctr < 100)
    {
        PerformPivotOperationsExtended(tableau);
        tempTab = tableau;

        PerformPivotOperationsExtended(tableau);

        allPosOrNeg = std::all_of(tableau[0].begin(), tableau[0].end() - 1, [](int num)
                                  { return num <= 0; });

        // if (allPosOrNeg && tableau[0].back() == 0)
        if (allPosOrNeg && tableau[0].back() == 0)
        {
            isSolved = true;
        }
        ctr++;
        // std::cout << (allPosOrNeg && tableau[0].back() == 0) << std::endl;
        // std::cout << tableau[0].back() << std::endl;
    }

    if (isSolved)
    {
        tableau = tempTab;
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

    maxObj = true;
    // // maxObj = false;

    // // TODO move this output to imgui
    // if (ctr == 100)
    // {
    //     std::cout << "unbounded" << std::endl;
    // }
}

void TwoPhase::PrepSolutionDisplay()
{
    PrepSolutionDisplayExtended();

    // move theta to the correct table for display
    for (int i = 1; i < static_cast<int>(tableaus.size()); i++)
    {
        for (int j = 1; j < static_cast<int>(tableaus[i].size()); j++)
        {
            tableaus[i - 1][j].back() = tableaus[i][j].back();
        }
    }

    // zero out theta column in last table
    // for (int j = 1; j < static_cast<int>(tableaus[0].size()); j++)
    // {
    //     tableaus.back()[j].back() = 0;
    // }

    std::cout << std::endl;

    for (const auto &table : tableaus)
    {
        for (const auto &row : table)
        {
            for (auto element : row)
            {
                std::cout << element << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}

void TwoPhase::PrepSolutionDisplayExtended()
{
    // move theta to the correct table for display
    for (int i = 2; i < static_cast<int>(tableausExtended.size()); i++)
    {
        for (int j = 1; j < static_cast<int>(tableausExtended[i].size()); j++)
        {
            tableausExtended[i - 2][j].back() = tableausExtended[i][j].back();
        }
    }

    // zero out theta column in last table
    // for (int j = 2; j < static_cast<int>(tableaus[0].size()); j++)
    // {
    //     tableausExtended.back()[j].back() = 0;
    // }

    std::cout << std::endl;

    for (const auto &table : tableausExtended)
    {
        for (const auto &row : table)
        {
            for (auto element : row)
            {
                std::cout << element << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}