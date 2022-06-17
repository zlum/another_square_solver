#include "producerconsumer/buffer.hpp"
#include "quadequation/quadprinter.h"
#include "quadequation/quadreader.h"
#include "quadequation/quadsolver.h"

#include <iostream>
#include <memory>
#include <thread>

using namespace std;

int main(int argc, char* argv[])
{
    // Create shared buffer objects to transfer tasks
    // from producers to consumers
    auto bufCoeffs = make_shared<Buffer<QuadCoeffs>>();
    auto bufEquation = make_shared<Buffer<QuadEquation>>();

    // Create ProducerConsumer instances to read, solve and print
    // quadratic equations
    QuadReader reader(argc, argv, bufCoeffs);
    QuadSolver solver1(bufCoeffs, bufEquation);
//    QuadSolver solver2(bufCoeffs, bufEquation);
    QuadPrinter printer(bufEquation);

    // Start worker threads of producers and consumers
    reader.start();
    solver1.start();
//    solver2.start();
    printer.start();

    // Stop threads after theirs work done
    reader.join();
    solver1.stopLater();
//    solver2.stopLater();
    solver1.join();
//    solver2.join();
    printer.stopLater();
    printer.join();

    // Producers and Consumers threads also joins in destructors

    return 0;
}
