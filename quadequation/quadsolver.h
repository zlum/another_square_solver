#pragma once

#include "producerconsumer/producerconsumer.h"

#include "producerconsumer/buffer.hpp"
#include "quadtypes.h"

#include <memory>

// Reads coefficients from the input buffer, calc roots and
// writes solution to the output buffer
class QuadSolver:
        public ProducerConsumer
{
public:
    explicit QuadSolver(std::shared_ptr<Buffer<QuadCoeffs>> inputBuf,
                        std::shared_ptr<Buffer<QuadEquation>> outputBuf);
    virtual ~QuadSolver();

    // ProducerConsumer override
    virtual void stopLater() override;

private:
    // ProducerConsumer override
    virtual void worker() override;

    // Calculate one root
    static double calcRoot(double sqrtDsc, bool isNegative, int a, int b);

    // Calculate roots of quadratic equation
    // Using NaN as no root mark
    static QuadRoots calcRoots(const QuadCoeffs& coeffs);
    // Calculate extremum of quadratic equation
    static QuadExtremum calcExtremum(const QuadCoeffs& coeffs);

private:
    std::shared_ptr<Buffer<QuadCoeffs>> _inputBuf;
    std::shared_ptr<Buffer<QuadEquation>> _outputBuf;
};
