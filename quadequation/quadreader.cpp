#include "quadreader.h"

#include <exception>
#include <string>
#include <utility>

using namespace std;

QuadReader::QuadReader(int argc,
                       char* argv[],
                       shared_ptr<Buffer<QuadCoeffs>> outputBuf):
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
    // Wake threads up to interrupt buffer operations
    _buf->notifyAll();
}

void QuadReader::worker()
{
    // Read 3 values per step. Skip argv[0] cause it is working directory
    size_t argNum = 1;
    const bool& workFlag = getWorkFlag();

    // Return if all arguments has been read or stopLater() is called
    while((argNum + _coeffsNumber < _argc + 1) && workFlag)
    {
        try
        {
            _buf->emplace(readCoeffs(argNum), getWorkFlag());
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

        argNum += _coeffsNumber; // Go to the next coefficients
    }
}

QuadCoeffs QuadReader::readCoeffs(size_t argNum)
{
    size_t i = 0; // Counter of correctly read values
    QuadCoeffs coeffs;

    // Read arguments until QuadCoeffs will be fulfilled
    while(i < _coeffsNumber)
    {
        coeffs.at(i) = std::stoi(_argv[argNum + i]); // Get next argument
        ++i;
    }

    // Return coefficients if all of them had been read correctly
    return coeffs;
}
