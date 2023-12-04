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

    constraints = {
        {2, 1, 100},
        {1, 1, 80},
        {1, 0, 40}};

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

    // convert objFunction to string vector
    std::vector<std::string> tempVec = {};
    for (const auto &item : objFunction)
    {
        tempVec.push_back(std::to_string(item));
        if (item != objFunction.back())
        {
            tempVec.push_back("+");
        }
    }
    canonical.push_back(tempVec);
    tempVec.clear();

    //TODO fix issue with the plus and add = to last element

    // convert constraints to string vector
    for (const auto &row : constraints)
    {
        for (float element : row)
        {
            tempVec.push_back(std::to_string(element));
            // if (element != objFunction.back())
            // {
                tempVec.push_back("+");
            // }
        }
        canonical.push_back(tempVec);
        tempVec.clear();
    }

    // canonical.insert(strVector.begin(), "z - ");
    canonical[0].insert(canonical[0].begin(), "z -");
    canonical[0].insert(canonical[0].end(), "= 0");

    for (int i = 1; i < canonical.size(); i++)
    {
        canonical[i].insert(canonical[i].end(), "s" + std::to_string(i));
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
}
