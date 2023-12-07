#include "PrimalSolver.hpp"

PrimalSolver::PrimalSolver()
{
    Init();
}

PrimalSolver::~PrimalSolver()
{
}

void PrimalSolver::Init()
{
    // TODO handle this as input from the user

    objFunction = {3, 2};

    // TODO have a check to see if there are only zeros and no 1s at the end of the user input indicating primal simplex
    // 0 or 1 at the end for <= or >= ... 0 being <= and 1 being >=
    constraints = {
        {2, 1, 100, 0},
        {1, 1, 80, 0},
        {1, 0, 40, 0},
    };

    // //extra test values
    // objFunction = {3, 2, 3};

    // constraints = {
    //     {2, 1, 3, 100, 0},
    //     {1, 1, 3, 80, 0},
    //     {1, 0, 3, 40, 0},
    //     {1, 2, 3, 50, 0},
    // };

    StandardForm();
    BuildTableauMathForm();
    PerformMathSteps();
}

void PrimalSolver::StandardForm()
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
            try
            {
                float floatValue = std::stof(element);
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
                continue;
            }
            catch (const std::out_of_range &)
            {
                // Handle the case where the converted float value is out of range
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

    // TODO move display to imgui not through cout

    // display the canonical form temporay here until implemented in imgui
    // std::cout << "canonical form:" << std::endl;
    // for (const auto &row : canonical)
    // {
    //     for (auto element : row)
    //     {
    //         std::cout << element << " ";
    //     }
    //     std::cout << std::endl;
    // }

    tempVec.clear();
    tempVec.shrink_to_fit();
    canonical.shrink_to_fit();
}

/**
 * Builds the tableau mathematical form for the PrimalSolver class.
 *
 * @param None
 *
 * @return None
 *
 * @throws None
 */
void PrimalSolver::BuildTableauMathForm()
{
    std::vector<std::vector<float>> constraintsTab = constraints;
    std::vector<float> objFunctionRow = objFunction;

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

    // TODO handle max or min
    for (auto &item : objFunctionRow)
    {
        item = -item;
    }

    // fill in objective function with 0s
    for (int i = 0; i < static_cast<int>(constraintsTab.size() + 1); i++)
    {
        objFunctionRow.push_back(0.0f);
    }

    // combine the tables
    tableauMathForm = constraintsTab;
    tableauMathForm.insert(tableauMathForm.begin(), objFunctionRow);

    // TODO move display to imgui not through cout
    std::cout << "display simple:" << std::endl;
    for (auto &row : tableauMathForm)
    {
        for (auto &item : row)
        {
            std::cout << item << " ";
        }
        std::cout << std::endl;
    }
}

/**
 * Performs the necessary mathematical steps.
 *
 * @param None
 *
 * @return None
 *
 * @throws None
 */
void PrimalSolver::PerformMathSteps()
{

    std::cout << "\nperforming steps" << std::endl;

    std::vector<std::vector<float>> tableauStageOne = tableauMathForm;
    std::vector<float> objFunction = tableauMathForm[0];

    // select column based on largest value
    // auto max = std::max_element(objFunction.begin(), objFunction.end());

    // int max = std::max(*std::max_element(objFunction.begin(), objFunction.end()), -*std::min_element(objFunction.begin(), objFunction.end()));
    // int pivotColumn{};
    // // if (max != objFunction.end())
    // // {
    // pivotColumn = std::distance(objFunction.begin(), max);
    // pivotColumn = std::distance(objFunction.begin(), std::max_element(objFunction.begin(), objFunction.end()));
    //     // std::cout << "The largest value is: " << *max << std::endl;
    // }
    // else
    // {
    //     std::cout << "no max found" << std::endl;
    // }

    // auto it = std::find(objFunction.begin(), objFunction.end(), max);
    // if (it != objFunction.end())
    // {
    //     int index = std::distance(objFunction.begin(), it);
    //     std::cout << "Index of element " << max << ": " << index << std::endl;
    // }
    // else
    // {
    //     std::cout << "Element not found" << std::endl;
    // }

    // pivotColumn = max;

    int max = std::max(*std::max_element(objFunction.begin(), objFunction.end()), -*std::min_element(objFunction.begin(), objFunction.end()));
    // int pivotColumn = std::distance(objFunction.begin(), std::max_element(objFunction.begin(), objFunction.end()));

    std::cout << "max: " << max << std::endl;

    // dont forget the sign check
    //  auto pivotColumnIterator = std::find(objFunction.begin(), objFunction.end(), -max);
    //  int pivotColumn = std::distance(objFunction.begin(), pivotColumnIterator);

    auto pivotColumnIterator = std::find_if(objFunction.begin(), objFunction.end(), [max](int value)
                                            { return std::abs(value) == std::abs(max); });
    int pivotColumn = std::distance(objFunction.begin(), pivotColumnIterator);
    std::cout << "max index: " << pivotColumn << std::endl;

    // calculate percentages in theta column
    int len{tableauStageOne.size()};
    for (int i = 1; i < len; i++)
    {
        tableauStageOne[i].push_back(tableauStageOne[i].back() / tableauStageOne[i][pivotColumn]);
    }

    // select row based on smallest value in last column (Theta)
    float smallest = tableauStageOne[1].back(); // Initialize with the first row's last element
    int pivotRow = 1;                           // Initialize with the index of the first row
    for (int i = 2; i < static_cast<int>(tableauStageOne.size()); i++)
    {
        float current = tableauStageOne[i].back(); // Get the last element of the current row
        if (current < smallest)
        {
            smallest = current; // Update the smallest if the current element is smaller
            pivotRow = i;       // Update the index of the smallest row
        }
    }
    // std::cout << "Smallest number: " << smallest << std::endl;
    // std::cout << "Row with the smallest number: " << pivotRow << std::endl;

    // move on to tableau two
    std::vector<std::vector<float>> tableauStageTwo = tableauStageOne;
    // std::vector<std::vector<float>> tableauStageTwo = {};
    // tableauStageTwo.reserve(tableauStageOne.size());

    // remove theta
    for (int i = 1; i < static_cast<int>(tableauStageTwo.size()); i++)
    {
        tableauStageTwo[i].pop_back();
    }

    // pivot row
    std::vector<float> pivotRowVec = {tableauStageTwo[pivotRow]};

    // pivot operation for new tableau primal simplex
    for (int i = 0; i < static_cast<int>(tableauStageOne.size()); i++)
    {
        for (int j = 0; j < static_cast<int>(tableauStageOne[i].size()); j++)
        {
            // Element_New_Table((i, j)) = Element_Old_Table((i, j)) - (Element_Old_Table((i, Pivot_column)) * Element_New_Table((Pivot_Row, j)))
            tableauStageTwo[i][j] = tableauStageOne[i][j] - tableauStageOne[i][pivotColumn] * tableauStageTwo[pivotRow][j];
        }
    }

    // add back the pivot row from being zeroed out
    tableauStageTwo.erase(tableauStageTwo.begin() + pivotRow);
    tableauStageTwo.insert(tableauStageTwo.begin() + pivotRow, pivotRowVec);

    std::cout << "display math:" << std::endl;

    for (auto &row : tableauStageTwo)
    {
        for (auto &item : row)
        {
            std::cout << item << " ";
        }
        std::cout << std::endl;
    }
}