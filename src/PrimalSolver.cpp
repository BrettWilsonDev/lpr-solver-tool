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

    objFunction = {3, 2};

    // 0 or 1 at the end for <= or >= ... 0 being <= and 1 being >=
    constraints = {
        {2, 1, 100, 0},
        {1, 1, 80, 0},
        {1, 0, 40, 0}};

    StandardForm();
}

void PrimalSolver::StandardForm()
{
    std::cout << "obj:" << std::endl;
    for (const auto &item : objFunction)
    {
        std::cout << item << " ";
    }
    std::cout << std::endl;

    std::cout << "constraints:" << std::endl;
    for (const auto &row : constraints)
    {
        for (float element : row)
        {
            std::cout << element << " ";
        }
        std::cout << std::endl;
    }

    // build canonical form for display

    // canonical form vector of strings
    std::vector<std::vector<std::string>> canonical;

    std::vector<std::string> tempVec = {};
    for (int i = 0; i < objFunction.size(); i++)
    {
        tempVec.push_back(std::to_string(objFunction[i]));
        if (i != objFunction.size() - 1)
        {
            tempVec.push_back("+");
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

    // turn the objective function into standard form
    canonical[0].insert(canonical[0].begin(), "z -");
    canonical[0].insert(canonical[0].end(), "= 0");

    // // remove the <= or >= indicators
    // for (int i = 1; i < canonical.size(); i++)
    // {
    //     canonical[i].pop_back();
    // }

    // // add slack vars to the constraints
    // for (int i = 1; i < canonical.size(); i++)
    // {
    //     canonical[i].insert(canonical[i].end() - 1, "s" + std::to_string(i));
    // }

    // // add = to last element for canonical
    // for (int i = 1; i < canonical.size(); i++)
    // {
    //     canonical[i].insert(canonical[i].end() - 1, "=");
    // }

    for (int i = 1; i < canonical.size(); i++)
    {
        // remove the <= or >= indicators
        canonical[i].pop_back();
        // add slack vars to the constraints
        canonical[i].insert(canonical[i].end() - 1, "s" + std::to_string(i));
        // add = to last element for canonical
        canonical[i].insert(canonical[i].end() - 1, "=");
    }

    // add + between each number
    bool isFirstRow = true;
    for (auto &row : canonical)
    {
        if (isFirstRow)
        {
            isFirstRow = false;
            continue;
        }

        for (auto i = row.begin() + 1; i != row.end(); std::advance(i, 2))
        {
            if (std::next(i) == row.end() - 1)
            {
                break;
            }
            i = row.insert(i, "+");
        }
    }

    std::cout << "canonical:" << std::endl;
    for (const auto &row : canonical)
    {
        for (auto element : row)
        {
            std::cout << element << " ";
        }
        std::cout << std::endl;
    }

    // for (int i = 0; i < canonical.size(); ++i)
    // {
    //     for (int j = 0; j < canonical[i].size(); ++j)
    //     {
    //         std::cout << canonical[i][j] << " ";
    //     }
    //     std::cout << std::endl;
    // }
}
