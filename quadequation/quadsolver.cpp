#include "quadsolver.h"

#include <cmath>
#include <limits>
#include <utility>

using namespace std;

QuadSolver::QuadSolver(shared_ptr<Buffer<unique_ptr<QuadCoeffs>>> inputBuf,
                       shared_ptr<Buffer<unique_ptr<QuadEquation>>> outputBuf):
    _inputBuf(move(inputBuf)),
    _outputBuf(move(outputBuf))
{
}

QuadSolver::~QuadSolver()
{
    // Join the worker thread to avoid worker() execution after this destructor
    join();
}

void QuadSolver::stopLater()
{
    // Set work flag as false
    ProducerConsumer::stopLater();
    // Ware threads up to interrupt buffer operations
    _inputBuf->notifyAll();
    _outputBuf->notifyAll();
}

void QuadSolver::worker()
{
    // Returns if stopLater() had been called and input buffer is empty
    const bool& workFlag = getWorkFlag();

    while(true)
    {
        unique_ptr<QuadCoeffs> coeffs = _inputBuf->getAndPop(workFlag);

        if(coeffs == nullptr)
        {
            // Interrupt called
            break;
        }

        // Create equation
        auto equation = make_unique<QuadEquation>();

        equation->coeffs = move(*coeffs);
        equation->roots = calcRoots(equation->coeffs);
        equation->extremum = calcExtremum(equation->coeffs);

        if(!_outputBuf->emplace(move(equation), workFlag))
        {
            // Interrupt called
            break;
        }
    }
}

double QuadSolver::calcRoot(double sqrtDsc, bool isNegative, int a, int b)
{
    return (-b + (isNegative ? -sqrtDsc : sqrtDsc)) / (2 * a);
}

QuadRoots QuadSolver::calcRoots(const QuadCoeffs& coeffs)
{
    // Get aliases for convenience
    const int& a = coeffs.at(0);
    const int& b = coeffs.at(1);
    const int& c = coeffs.at(2);

    // Check if it is a quadratic equation
    if(a == 0)
    {
        if(b == 0)
        {
            // Both x coefficients are zero
            return
            {
                numeric_limits<double>::quiet_NaN(),
                numeric_limits<double>::quiet_NaN()
            };
        }

        // Calc as simple bx + c = 0 equation
        return {b / -c, numeric_limits<double>::quiet_NaN()};
    }

    auto discriminant = double((b * b) - (4 * a * c));

    // Determines roots count by discriminant sign
    if(discriminant != FP_ZERO && discriminant > FP_ZERO)
    {
        // Two roots
        double sqrtDsc = sqrt(discriminant);

        return
        {
            calcRoot(sqrtDsc, true, a, b),
            calcRoot(sqrtDsc, false, a, b)
        };
    }

    if(discriminant == FP_ZERO)
    {
        // One root
        return
        {
            calcRoot(sqrt(discriminant), false, a, b),
            numeric_limits<double>::quiet_NaN()
        };
    }

    // No roots
    return
    {
        numeric_limits<double>::quiet_NaN(),
        numeric_limits<double>::quiet_NaN()
    };
}

QuadExtremum QuadSolver::calcExtremum(const QuadCoeffs& coeffs)
{
    // Get aliases for convenience
    const int& a = coeffs.at(0);
    const int& b = coeffs.at(1);

    return double(-b) / (2 * a);
}
