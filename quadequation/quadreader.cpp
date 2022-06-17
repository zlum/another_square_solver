#include "quadreader.h"

#include <exception>
#include <string>
#include <utility>

using namespace std;

QuadReader::QuadReader(int argc, char* argv[],
                       shared_ptr<Buffer<unique_ptr<QuadCoeffs>>> outputBuf):
    _argc(size_t(argc)),
    _argv(argv),
    _buf(move(outputBuf))
{
}

QuadReader::~QuadReader()
{
    // Join the worker thread to avoid worker() execution after this destructor
    join();
}

void QuadReader::stopLater()
{
    // Set work flag as false
    ProducerConsumer::stopLater();
    // Ware threads up to interrupt buffer operations
    _buf->notifyAll();
}

void QuadReader::worker()
{
    // Read 3 values per step. Skip argv[0] cause it is working directory
    // Return if all arguments has been read
    size_t arg = 1;

    while(arg + _coeffsNumber < _argc + 1)
    {
        unique_ptr<QuadCoeffs> coeffs;

        try
        {
            coeffs = readCoeffs(arg);
            _buf->emplace(move(coeffs), getWorkFlag());
        }
        catch(const invalid_argument&)
        {
        }
        catch(const out_of_range&)
        {
        }
        catch(const BufferOperationInterrupted&)
        {
            // Interrupt called
            break;
        }

        arg += _coeffsNumber; // Go to the next coefficients
    }
}

unique_ptr<QuadCoeffs> QuadReader::readCoeffs(size_t arg)
{
    size_t i = 0; // Counter of correctly read values
    auto coeffs = make_unique<QuadCoeffs>();

    // Read arguments until QuadCoeffs will be fulfilled
    while(i < 3)
    {
        coeffs->at(i) = std::stoi(_argv[arg + i]); // Get next argument
        ++i;
    }

    // Return coefficients if all of them had been read correctly
    return coeffs;
}
