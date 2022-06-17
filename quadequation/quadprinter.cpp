#include "quadprinter.h"

#include <cmath>
#include <iostream>

using namespace std;

QuadPrinter::QuadPrinter(shared_ptr<Buffer<QuadEquation>> inputBuf):
    _buf(move(inputBuf))
{
}

QuadPrinter::~QuadPrinter()
{
    // Join the worker thread to avoid worker() execution after this destructor
    join();
}

void QuadPrinter::stopLater()
{
    // Set work flag as false
    ProducerConsumer::stopLater();
    // Wake threads up to interrupt buffer operations
    _buf->notifyAll();
}

void QuadPrinter::worker()
{
    const bool& workFlag = getWorkFlag();

    // Breaks if stopLater() is called
    while(workFlag)
    {
        try
        {
            QuadEquation equation = _buf->getAndPop(getWorkFlag());
            printQuadEquation(equation);
        }
        catch(const BufferOperationInterrupted&)
        {
            // Interrupt called
            break;
        }
    }
}

void QuadPrinter::printQuadEquation(const QuadEquation& equation)
{
    // Print coefficients
    cout << '('
         << equation.coeffs.at(0) << ' '
         << equation.coeffs.at(1) << ' '
         << equation.coeffs.at(2)
         << ") => ";

    // Print roots
    // Using NaN as no root mark
    if(isnan(equation.roots.first))
    {
        cout << "no roots" << endl;
    }
    else
    if(isnan(equation.roots.second))
    {
        // One root
        cout << "root "
             << '('
             << equation.roots.first
             << ')';
    }
    else
    {
        // Two roots
        cout << "roots "
             << '('
             << equation.roots.first << ", "
             << equation.roots.second
             << "), ";
    }

    // Print extremum
    cout << "extremum "
         << '('
         << equation.extremum << ')'
         << endl;
}
