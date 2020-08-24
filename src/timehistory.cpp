/*
 * This is a straightforward application of the DoublePendulum classes:
 * given the parameters of a double pendulum and an initali condition
 * its equation of motion is numerically solved with a Runge-Kutta
 * algorithm and the coordinates of the notable points are saved in a
 * text file which can be later used to generate an animation with
 * other tools. 
 */

#define _USE_MATH_DEFINES
#include <cmath>
#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include "DoublePendulum/DoublePendulum.hpp"
#include "DoublePendulum/SimpleDoublePendulum.hpp"
#include "DoublePendulum/CompoundDoublePendulum.hpp"

const double g = 9.81;

void printHelpMessage() {
    std::cout << "Usage:" << std::endl << std::endl;
    std::cout << program_invocation_name << " M1 M2 L1 L2 ai1 ai2 wi1 wi2 type dt nStepMax" << std::endl << std::endl;
    std::cout << "\toutFile:    output file name." << std::endl;
    std::cout << "\ttype:       type of pendulum. One of [simple, compound]." << std::endl;
    std::cout << "\tM1, M2:     masses of the rods in [kg]." << std::endl;
    std::cout << "\tL1, L2:     lengths of the rods in [m]." << std::endl;
    std::cout << "\tai1, ai2:   starting angles of the rods with respect to the downward vertical position in [rad]." << std::endl;
    std::cout << "\twi1, wi2:   starting angular velocities of the rods in [rad/s]." << std::endl;
    std::cout << "\tdt:         time step of the simulation in [s]." << std::endl;
    std::cout << "\tnStepMax:   maximum number of steps of the simulation." << std::endl << std::endl;
}

int main(int argc, const char * argv[])
{
    std::string outFileName, systemType;
    double M1, M2, L1, L2;
    double ai1, ai2, wi1, wi2;
    double dt;
    int nStepMax;
    bool simplePendulum;
    std::unique_ptr<DoublePendulum> pendulum;

    if (argc != 13) {
        std::cout << "Wrong number of arguments!" << std::endl << std::endl;
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
    systemType = std::string(argv[2]);
    if (systemType == "simple") {
        simplePendulum = true;
    } else if (systemType == "compound") {
        simplePendulum = false;
    } else {
        std::cerr << "Invalid type parameter!" << std::endl << std::endl;
        printHelpMessage();
        return 1;
    }
    // System parameters.
    M1 = std::stof(argv[3]);
    M2 = std::stof(argv[4]);
    L1 = std::stof(argv[5]);
    L2 = std::stof(argv[6]);
    ai1 = std::stof(argv[7]);
    ai2 = std::stof(argv[8]);
    wi1 = std::stof(argv[9]);
    wi2 = std::stof(argv[10]);
    // Environment parameters.
    dt = std::stof(argv[11]);
    nStepMax = std::stoi(argv[12]);

    // Output stream
    std::ofstream outFile(outFileName);

    // Choose pendulum type and initialize the object.
    if (simplePendulum) {
        pendulum = std::make_unique<SimpleDoublePendulum>(M1, M2, L1, L2, dt, g);
    } else {
        pendulum = std::make_unique<CompoundDoublePendulum>(M1, M2, L1, L2, dt, g);
    }

    StateVector currState, nextState;

    // Initial state.
    currState.a1 = ai1;
    currState.w1 = wi1;
    currState.a2 = ai2;
    currState.w2 = wi2;

    for (int i = 0; i < nStepMax - 1; i++) {
        nextState = pendulum->calcNextState(currState);
        
        for (int j = 0; j < pendulum->N_STATE_VARS; j++) {
            currState[j] = nextState[j];
        }
        outFile << pendulum->getTextOutput(currState);
    }
}