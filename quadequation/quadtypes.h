#pragma once

#include <array>
#include <utility>

using QuadCoeffs = std::array<int, 3>;
using QuadRoots = std::pair<double, double>;
using QuadExtremum = double;

// Input coefficients and solution of quadratic equation
struct QuadEquation
{
    QuadCoeffs coeffs;
    QuadRoots roots;
    QuadExtremum extremum;
};
