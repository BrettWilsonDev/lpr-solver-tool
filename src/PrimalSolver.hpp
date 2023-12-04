#pragma once

#include <iostream>
#include <vector>
#include <string>

class PrimalSolver
{
public:
    PrimalSolver();
    ~PrimalSolver();
    void test();
private:
    void Init();
    void StandardForm();
    void Solve();
    void PrintSolution();

    std::vector<float> objFunction;
    // std::vector<float> Constraints = {2, 1, 1, 1, 1, 0,};
    std::vector<std::vector<float>> constraints;

    
};
