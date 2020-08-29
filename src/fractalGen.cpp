#include <string>
#include <iostream>
#include <memory>
#include "DoublePendulum/DoublePendulum.hpp"
#include "Fractal/Fractal.hpp"
#include "Fractal/UniformGrid.hpp"

const double g = 9.81;

void printHelpMessage() {
    std::cout << "Usage:" << std::endl << std::endl;
    std::cout << program_invocation_name << " outFile pendulumType M1 M2 L1 L2 ai1Min aiMax ai2Min ai2Max gridSize dt nStepMax" << std::endl << std::endl;
    std::cout << "\toutFile:    output file name." << std::endl;
    std::cout << "\tpendulumType:" << std::endl;
    std::cout << "              type of pendulum. One of [simple, compound]." << std::endl;
    std::cout << "\tM1, M2:     masses of the rods in [kg]." << std::endl;
    std::cout << "\tL1, L2:     lengths of the rods in [m]." << std::endl;
    std::cout << "\tai1Min, ai1Max, ai2Min, ai2Max:" << std::endl;
    std::cout << "\t            ranges for the starting angles of the rods with respect to the downward vertical position in [rad]." << std::endl;
    std::cout << "\tgridSize:   increment of the starting angles in [rad]." << std::endl;
    std::cout << "\tdt:         time step of the simulation in [s]." << std::endl;
    std::cout << "\tnStepMax:   maximum number of steps of the simulation." << std::endl << std::endl;
}

int main(int argc, const char * argv[])
{
    std::string outFileName, pendulumTypeStr;
    DoublePendulum::Variant pendulumType;
    double M1, M2, L1, L2;
    double ai1Min, ai1Max, ai2Min, ai2Max;
    double dt, gridSize;
    int nStepMax;

    if (argc != 14) {
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
    ai1Min = std::stof(argv[7]);
    ai1Max = std::stof(argv[8]);
    ai2Min = std::stof(argv[9]);
    ai2Max = std::stof(argv[10]);
    gridSize = std::stof(argv[11]);
    dt = std::stof(argv[12]);
    nStepMax = std::stoi(argv[13]);

    UniformGrid grid(
        std::make_shared<Fractal> (
            DoublePendulum::makeDoublePendulum(
                M1, M2, L1, L2, dt, g, pendulumType
            )
        ),
        nStepMax, ai1Min, ai1Max, ai2Min, ai2Max, gridSize
    );

    grid.calcData();
    grid.saveData(outFileName);
}