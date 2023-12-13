#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <cstdlib>

class TwoPhase
{
public:
    TwoPhase();
    ~TwoPhase();
    std::vector<std::vector<std::string>> GetCanonicalForm() { return canonical; }
    // std::vector<std::vector<std::string>> GetTableau() { return canonical; }

private:
    void Init();
    void StandardForm();
    void standardFormExtended();
    void BuildTableauMathForm();
    std::vector<std::vector<float>> BuildTableauMathFormExtended();
    void PerformPivotOperations(std::vector<std::vector<float>> tab);
    void PerformPivotOperationsExtended(std::vector<std::vector<float>> tab);
    void Solve();
    void SolveExtended();
    void PrepSolutionDisplay();
    void PrepSolutionDisplayExtended();

    bool maxObj{};

    std::vector<float> objFunction;
    std::vector<std::vector<float>> constraints;
    // canonical form vector of strings
    std::vector<std::vector<std::string>> canonical;
    // tableau math form
    std::vector<std::vector<float>> tableauMathForm;
    std::vector<std::vector<float>> tableau;

    std::vector<std::vector<std::vector<float>>> tableaus;
    std::vector<std::vector<std::vector<float>>> tableausExtended;
};
