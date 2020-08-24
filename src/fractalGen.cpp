/*
 * It is possible to draw a fractal by evaluating after how much time
 * a double pendulum system has one of its rod flip vertically while
 * varying its initial state.
 * 
 * This program simulates the motion of a double pendulum system using
 * a Runge-Kutta method to numerically solve its equation of motion
 * for the whole space of initial conditions (given by the absolute
 * rotation of the two rods from -PI to +PI), discretized as a simple
 * grid of fixed size delta_ai is used.
 * 
 * Possible future improvements:
 *  - better (more efficient) numerical solver
 *  - better discretization. At the moment the program loses A LOT of
 *    time in the regions where no flip occurs in reasonable time: it
 *    would be nice to have the resolution increase only in the more
 *    interesting zones of the fractal.
 * 
 * Source: https://www.famaf.unc.edu.ar/~vmarconi/fiscomp/Double.pdf
 */

#define _USE_MATH_DEFINES
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <chrono>
#include "DoublePendulum/DoublePendulum.hpp"
#include "DoublePendulum/SimpleDoublePendulum.hpp"
#include "DoublePendulum/CompoundDoublePendulum.hpp"

const double g = 9.81;

/*
 * Detect wether one of the two rods flipped between the two given states.
 * 
 * A rod flips when it passes through the vertical upwards (alpha = +/- pi) clockwise
 * or anticlockwise.
 * This can be detected by counting the number of complete circles made by each rod:
 * if it differs between the two states then the rod flipped between those states.
 */
bool detectFlip(StateVector prevState, StateVector currState) {
    // The offset by PI is to start counting rounds at the top (at an agle of PI radians
    // in the global reference system) instead of at the bottom (0 radians).
    float nRoundsRod1PrevState = floor((prevState.a1 - M_PI) / (2 * M_PI));
    float nRoundsRod1CurrState = floor((currState.a1 - M_PI) / (2 * M_PI));
    float nRoundsRod2PrevState = floor((prevState.a2 - M_PI) / (2 * M_PI));
    float nRoundsRod2CurrState = floor((currState.a2 - M_PI) / (2 * M_PI));

    return (nRoundsRod1PrevState != nRoundsRod1CurrState) || (nRoundsRod2PrevState != nRoundsRod2CurrState);
}

void printHelpMessage() {
    std::cout << "Usage:" << std::endl << std::endl;
    std::cout << program_invocation_name << " outFile systemType M1 M2 L1 L2 ai1Min aiMax ai2Min ai2Max delta_ai dt nStepMax" << std::endl << std::endl;
    std::cout << "\toutFile:    output file name." << std::endl;
    std::cout << "\tsystemType: type of pendulum. One of [simple, compound]." << std::endl;
    std::cout << "\tM1, M2:     masses of the rods in [kg]." << std::endl;
    std::cout << "\tL1, L2:     lengths of the rods in [m]." << std::endl;
    std::cout << "\tai1Min, ai1Max, ai2Min, ai2Max:" << std::endl;
    std::cout << "\t            ranges for the starting angles of the rods with respect to the downward vertical position in [rad]." << std::endl;
    std::cout << "\tdelta_ai:   increment of the starting angles in [rad]." << std::endl;
    std::cout << "\tdt:         time step of the simulation in [s]." << std::endl;
    std::cout << "\tnStepMax:   maximum number of steps of the simulation." << std::endl << std::endl;
}

int main(int argc, const char * argv[])
{
    std::string outFileName, systemType;
    double M1, M2, L1, L2;
    double ai1Min, ai1Max, ai2Min, ai2Max;
    double dt, delta_ai;
    int nStepMax;
    bool simplePendulum;
    std::unique_ptr<DoublePendulum> pendulum;

    auto chronoStart = std::chrono::high_resolution_clock::now();

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
    M1 = std::stof(argv[3]);
    M2 = std::stof(argv[4]);
    L1 = std::stof(argv[5]);
    L2 = std::stof(argv[6]);
    // Environment parameters.
    ai1Min = std::stof(argv[7]);
    ai1Max = std::stof(argv[8]);
    ai2Min = std::stof(argv[9]);
    ai2Max = std::stof(argv[10]);
    delta_ai = std::stof(argv[11]);
    dt = std::stof(argv[12]);
    nStepMax = std::stoi(argv[13]);

    int imgSize[] = { (int) ceil((ai1Max - ai1Min) / delta_ai), (int) ceil((ai2Max - ai2Min) / delta_ai) };

    std::cout << "Final image resolution approx: " << imgSize[0] << "X" << imgSize[1] << " px" << std::endl;

    std::ofstream outFile(outFileName);

    // Write simulation parameters in the header.
    outFile << "#M1\tM2\tL1\tL2\tai1Min\tai1Max\tai2Min\tai2Max\tdelta_ai\ttype\tdt\tnStepMax\timgSizeX\timgSizeY\n";
    outFile << "#" << M1<< "\t" << M2<< "\t" << L1<< "\t" << L2<< "\t" << ai1Min<< "\t" << ai1Max<< "\t" << ai2Min<< "\t" << ai2Max<< "\t" << delta_ai<< "\t" << systemType << "\t" << dt<< "\t" << nStepMax << imgSize[0] << "\t" << imgSize[1] << "\n";
    outFile << "#RENDER_TYPE:NORMAL" << std::endl;

    // Choose pendulum type and initialize.
    if (simplePendulum) {
        pendulum = std::make_unique<SimpleDoublePendulum>(M1, M2, L1, L2, dt, g);
    } else {
        pendulum = std::make_unique<CompoundDoublePendulum>(M1, M2, L1, L2, dt, g);
    }

    double ai1, ai2;
    int i, j, count;
    StateVector currState, nextState;
    
    i = 0;
    ai1 = ai1Min;
    while (ai1 < ai1Max) {
        ai1 = ai1Min + i * delta_ai;

        j = 0;
        ai2 = ai2Min;
        while (ai2 < ai2Max) {
            ai2 = ai2Min + j * delta_ai;
            // If this condition is not met then it is physically impossible for any rod to flip.
            // See: http://csaapt.org/uploads/3/4/4/2/34425343/csaapt_maypalace_sp16.pdf
            if (3 * L1 * cos(ai1) + L2 * cos(ai2) < 2) {

                // Initial state.
                currState.a1 = ai1;
                currState.w1 = 0;
                currState.a2 = ai2;
                currState.w2 = 0;
                // Numerically solve the state equation using the Runge-Kutta method.
                for (count = 0; count < nStepMax; count++) {
                    nextState = pendulum->calcNextState(currState);

                    // Check if a flip happened between the last two states.
                    if (count > 1 && detectFlip(currState, nextState)) {
                        break;
                    }

                    // Update the current state.
                    currState = nextState;
                }
                outFile << i << "\t" << j << "\t" << count << std::endl;
            }
            j += 1;
        }
        i += 1;
    }

    auto chronoStop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(chronoStop - chronoStart);
    outFile << "# Elapsed time: " << duration.count() << " s";

}