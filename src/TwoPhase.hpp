#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include "PrimalTwoPhaseBase.hpp"

class TwoPhase : public PrimalTwoPhaseBase
{
public:
    TwoPhase();
    ~TwoPhase();
    // std::vector<std::vector<std::string>> GetCanonicalForm() { return canonical; }
    // std::vector<std::vector<std::string>> GetTableau() { return canonical; }

    void Init();
    void standardFormExtended() override;
    std::vector<std::vector<float>> BuildTableauMathFormExtended() override;
    void PerformPivotOperationsExtended(std::vector<std::vector<float>> tab) override;
    void SolveExtended() override;
    void PrepSolutionDisplayExtended() override;
private:
};
