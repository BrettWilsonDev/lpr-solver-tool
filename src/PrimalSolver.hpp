#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include "PrimalTwoPhaseBase.hpp"

class PrimalSolver : public PrimalTwoPhaseBase
{
public:
    PrimalSolver();
    ~PrimalSolver();
    // std::vector<std::vector<std::string>> GetCanonicalForm() { return canonical; }
    // std::vector<std::vector<std::string>> GetTableau() { return canonical; }
    void Init();

private:
};
