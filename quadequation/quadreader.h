#pragma once

#include "producerconsumer/producerconsumer.h"

#include "producerconsumer/buffer.hpp"
#include "quadtypes.h"

#include <cstddef>
#include <memory>
#include <stdexcept>

class BigNumberBuilder;

// Reads coefficients from cli, stores tasks and
// writes them to the output buffer
class QuadReader:
        public ProducerConsumer
{
public:
    explicit QuadReader(int argc, char* argv[],
                        std::shared_ptr<Buffer<std::unique_ptr<QuadCoeffs>>>
                            outputBuf);
    virtual ~QuadReader();

    // ProducerConsumer override
    virtual void stopLater() override;

private:
    // ProducerConsumer override
    virtual void worker() override;

    // Read coefficients of quadratic equation from cli arguments
    std::unique_ptr<QuadCoeffs> readCoeffs(size_t arg);

private:
    size_t _argc;
    char** _argv;
    std::shared_ptr<Buffer<std::unique_ptr<QuadCoeffs>>> _buf;
    static constexpr int _coeffsNumber = 3;
};
