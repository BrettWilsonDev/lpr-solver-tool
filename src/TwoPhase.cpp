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
    //     {0, 7, -8, 0},
    // };

    // objFunction = {100, 30, 20};

    // constraints = {
    //     {2, 3, 4, 4, 1},
    //     {5, -6, 7, 4, 2},
    //     {5, -6, 7, 4, 1},
    //     {0, 7, -8, 4, 2},
    // };

    // StandardForm();
    BuildTableauMathForm();
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

    BuildTableauMathFormExtended();

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
    tableaus.push_back(tableauMathForm);
}

void TwoPhase::BuildTableauMathFormExtended()
{
    std::vector<std::vector<float>> tempTableau = tableauMathForm;
    std::vector<std::vector<float>> temp;
    std::vector<float> tempObj;

    // start the array by adding the vars
    for (int i = 0; i < static_cast<int>(constraints.size()); i++)
    {
        // TODO handle min
        if (constraints[i].back() == 1 || constraints[i].back() == 2)
        {
            temp.push_back(constraints[i]);
        }
    }

    // build the constraints function in math form
    for (int i = 0; i < static_cast<int>(temp.size()); i++)
    {
        float tempNum{};
        tempNum = temp[i][temp[i].size() - 2];
        temp[i].pop_back();
        temp[i].pop_back();
        temp[i].push_back(0);
        temp[i].push_back(-1);
        temp[i].push_back(tempNum);
    }

    //count amount of = constraints
    int ctr{};
    for (int i = 0; i < static_cast<int>(constraints.size()); i++)
    {
        if (constraints[i].back() == 2)
        {
            ctr++;
        }
    }

    // make a dummy row of zeros to avoid out of bounds errors
    temp.push_back(std::vector<float>());
    for (int i = 0; i < static_cast<int>(temp[0].size()); i++)
    {
        temp.back().push_back(0);
    }

    // convert all the negative numbers to positive
    for (int i = 0; i < static_cast<int>(temp.size() - 1); i++)
    {
        for (int j = 0; j < static_cast<int>(temp[i].size() - 1); j++)
        {
            if (temp[i][j] != 0)
            {
                temp[i][j] = -temp[i][j];
            }
        }
    }

    // do the arithmetic for the w function
    for (int j = 0; j < static_cast<int>(temp[0].size()); j++)
    {
        float sum{};
        for (int i = 0; i < static_cast<int>(temp.size()); i++)
        {
            sum += temp[i][j];
        }
        tempObj.push_back(sum);
        // std::cout << "Sum of column " << j << ": " << sum << std::endl;
    }

    int len = abs(tempObj[tempObj.size() - 2]) - ctr;
    // tempObj.erase(tempObj.end() - 2);
    float tempObjNum{};
    tempObjNum = tempObj.back();
    tempObj.pop_back();
    tempObj.pop_back();

    for (int i = 0; i < len; i++)
    {
        tempObj.push_back(1);
    }

    //count amount of slack vars
    ctr = 0;
    for (int i = 0; i < static_cast<int>(constraints.size()); i++)
    {
        // TODO handle min
        if (constraints[i].back() == 0)
        {
            ctr++;
        }
    }
    
    // add slack vars
    for (int i = 0; i < ctr; i++)
    {
        tempObj.push_back(0);
    }

    // add back the last var
    tempObj.push_back(tempObjNum);

    // convert the sign back
    for (int j = 0; j < static_cast<int>(tempObj.size() - 1); j++)
    {
        if (tempObj[j] != 0)
        {
            tempObj[j] = -tempObj[j];
        }
    }

    //add w function on top of objective function
    tempTableau.insert(tempTableau.begin(), tempObj);

    // servers a test display
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
}