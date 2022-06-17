#pragma once

#include "producerconsumer/producerconsumer.h"

#include "producerconsumer/buffer.hpp"
#include "quadtypes.h"

#include <memory>

// Extracts quadratic equations from the input buffer and
// prints them to the screen
class QuadPrinter:
        public ProducerConsumer
{
public:
    explicit QuadPrinter(std::shared_ptr<Buffer<QuadEquation>> inputBuf);
    virtual ~QuadPrinter();

    // ProducerConsumer override
    virtual void stopLater() override;

private:
    // ProducerConsumer override
    virtual void worker() override;

    // Print quadratic equation in the next format:
    // (a b c) => roots (x1, x2), extremum (m)
    static void printQuadEquation(const QuadEquation& equation);

private:
    std::shared_ptr<Buffer<QuadEquation>> _buf;
};
