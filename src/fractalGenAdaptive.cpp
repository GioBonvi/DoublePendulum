#include <string>
#include <iostream>
#include <memory>
#include "DoublePendulum/DoublePendulum.hpp"
#include "Fractal/Fractal.hpp"
#include "Fractal/Adaptive/AdaptiveGrid.hpp"

const double g = 9.81;

void printHelpMessage() {
    std::cout << "Usage:" << std::endl << std::endl;
    std::cout << program_invocation_name << " outFile systemType M1 M2 L1 L2 ai1Central ai2Central dt nStepMax nCycles nCyclesPrint" << std::endl << std::endl;
    std::cout << "\toutFile:       output file name." << std::endl;
    std::cout << "\tsystemType:    type of pendulum. One of [simple, compound]." << std::endl;
    std::cout << "\tM1, M2:        masses of the rods in [kg]." << std::endl;
    std::cout << "\tL1, L2:        lengths of the rods in [m]." << std::endl;
    std::cout << "\tai1Central, ai2Central:" << std::endl;
    std::cout << "\t               central values of the starting angles of the rods with respect to the downward vertical position in [rad]." << std::endl;
    std::cout << "\taiSize:        length of the square defining the ai domani in [rad]." << std::endl;
    std::cout << "\tdt:            time step of the simulation in [s]." << std::endl;
    std::cout << "\tnStepMax:      maximum number of steps for each simulation." << std::endl;
    std::cout << "\tnCycles:       number of cycles (increasing resolution of a region) to run." << std::endl;
    std::cout << "\tnCyclesPrint:  number of cycles after which a file with the partial data is printed. Defaults to 0 (never)." << std::endl << std::endl;
}

int main(int argc, const char * argv[])
{
    std::string outFileName, pendulumTypeStr;
    DoublePendulum::Variant pendulumType;
    double M1, M2, L1, L2;
    double ai1Central, ai2Central, aiSize;
    double dt;
    int nStepMax, nCycles, nCyclesPrint;

    // PARAMETERS.

    if (argc != 14 && argc != 13) {
        std::cerr << "Wrong number of arguments!" << std::endl << std::endl;
        printHelpMessage();
        return 1;
    }

    // Output file
    outFileName = std::string(argv[1]);
    if (outFileName.empty()) {
        std::cerr << "Empty output file name!" << std::endl << std::endl;
        printHelpMessage();
        return 1;
    }
    // Physical system parameters.
    pendulumTypeStr = std::string(argv[2]);
    if (pendulumTypeStr == "simple") {
        pendulumType = DoublePendulum::Variant::Simple;
    } else if (pendulumTypeStr == "compound") {
        pendulumType = DoublePendulum::Variant::Compound;
    } else {
        std::cerr << "Invalid type parameter!" << std::endl << std::endl;
        printHelpMessage();
        return 1;
    }

    M1 = std::stof(argv[3]);
    M2 = std::stof(argv[4]);
    L1 = std::stof(argv[5]);
    L2 = std::stof(argv[6]);
    // Environment parameters.
    ai1Central = std::stof(argv[7]);
    ai2Central = std::stof(argv[8]);
    aiSize = std::stof(argv[9]);
    dt = std::stof(argv[10]);
    nStepMax = std::stoi(argv[11]);
    nCycles = std::stoi(argv[12]);
    // Optional last argument.
    if (argc == 14) {
        nCyclesPrint = std::stoi(argv[13]);
    } else {
        nCyclesPrint = 0;
    }

    AdaptiveGrid grid(
        std::make_unique<Fractal> (
            DoublePendulum::makeDoublePendulum(M1, M2, L1, L2, dt, g, pendulumType)
        ),
        nStepMax, ai1Central, ai2Central, aiSize
    );

    if (nCyclesPrint > 0) {
        int cycles = 0;
        // Perform the calculations in batches of nCyclesPrint each...
        while (nCycles - cycles > nCyclesPrint) {
            grid.cycle(nCyclesPrint);
            cycles += nCyclesPrint;
            // ... print the intermediate restults...
            grid.saveImage(outFileName);
        }
        // ... perform the last calculations and print the final results.
        grid.cycle(nCycles - cycles);
        grid.saveImage(outFileName);
    } else {
        // Perform all the calculations...
        grid.cycle(nCycles);
        // ... then print the final result.
        grid.saveImage(outFileName);
    }
}