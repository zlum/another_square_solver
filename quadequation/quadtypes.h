#pragma once

#include <array>
#include <utility>

// Array with coefficients of quadratic equation
using QuadCoeffs = std::array<int, 3>;
using QuadRoots = std::pair<double, double>;
using QuadExtremum = double;

// Struct with coefficients and roots of quadratic equation
struct QuadEquation
{
    QuadCoeffs coeffs;
    QuadRoots roots;
    QuadExtremum extremum;
};
