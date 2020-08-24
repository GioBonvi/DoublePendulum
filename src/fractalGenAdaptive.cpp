#define _USE_MATH_DEFINES
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <chrono>
#include <set>
#include <array>
#include "DoublePendulum/DoublePendulum.hpp"
#include "DoublePendulum/SimpleDoublePendulum.hpp"
#include "DoublePendulum/CompoundDoublePendulum.hpp"
#include "AdaptiveCalculation/DataRegion.hpp"

const double g = 9.81;

/**
 * Custom comparator to compare pointers of any type.
 * 
 * Note: the type must be comparable itself.
 */
class ComparePointers {
    public:
        template<typename T>
        bool operator()(T *a, T *b) {
            return (*a) < (*b);
        }
};

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

int stepsToFlip(double ai1, double ai2, std::unique_ptr<DoublePendulum> &pendulum, int nStepMax) {
    int count;
    StateVector currState, nextState;
    
    // Initial state.
    currState.a1 = ai1;
    currState.w1 = 0;
    currState.a2 = ai2;
    currState.w2 = 0;

    // If this condition is not met then it is physically impossible for any rod to flip.
    // See: http://csaapt.org/uploads/3/4/4/2/34425343/csaapt_maypalace_sp16.pdf
    if (3 * pendulum->L1 * cos(currState[0]) + pendulum->L2 * cos(currState[2]) > 2) {
        return nStepMax - 1;
    }

    // Numerically solve the state equation using the Runge-Kutta method.
    for (count = 0; count < nStepMax; count++) {
        nextState = pendulum->calcNextState(currState);

        // Check if a flip happened between the last two states.
        if (count > 1 && detectFlip(currState, nextState)) {
            return count;
        }

        // Update the current state.
        currState = nextState;
    }

    return nStepMax - 1;
}

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
    std::cout << "\tnCyclesPrint:  number of cycles after which a file with the partial data is printed. Defaults to -1 (never)." << std::endl << std::endl;
}

int main(int argc, const char * argv[])
{
    std::string outFileName, systemType;
    double M1, M2, L1, L2;
    double ai1Central, ai2Central, aiSize;
    double dt;
    int nStepMax, nCycles, nCyclesPrint;
    bool simplePendulum;
    std::unique_ptr<DoublePendulum> pendulum;

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
    systemType = argv[2];
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
        nCyclesPrint = -1;
    }

    // SETUP

    auto chronoStart = std::chrono::high_resolution_clock::now();
    
    std::ofstream outFile(outFileName);

    // Write simulation parameters in the header.
    outFile << "#M1\tM2\tL1\tL2\ttype\tai1Central\tai2Central\taiSize\tdt\tnStepMax\tnCycles\n";
    outFile << "#" << M1 << "\t" << M2 << "\t" << L1 << "\t" << L2 << "\t" << systemType << "\t" << ai1Central << "\t" << ai2Central << "\t" << aiSize << "\t" << dt<< "\t" << nStepMax << "\t" << nCycles << "\n";
    outFile << "#RENDER_TYPE:ADAPTIVE" << std::endl;

    // Choose the pendulum type and initialize the variable.
    if (simplePendulum) {
        pendulum = std::make_unique<SimpleDoublePendulum>(M1, M2, L1, L2, dt, g);
    } else {
        pendulum = std::make_unique<CompoundDoublePendulum>(M1, M2, L1, L2, dt, g);
    }

    // CALCULATIONS

    // Multiset keeps the regions ordered by priority value, so
    // std::prev(regions.end()) always is the region with highest priority.
    // Note: a custom comparator is adopted to compare pointers.
    std::multiset<DataRegion*, ComparePointers> regions;
    std::array<DataRegion*, DataRegion::DATA_POINTS_N> newRegions;

    // The first region covers the whole domanin: subregions will be defined
    // automatically around the most "interesting" areas.
    regions.insert(new DataRegion(
        ai1Central,
        ai2Central,
        aiSize,
        aiSize,
        // Lambda expression to fit the f(x, y) format required by DataRegion.
        [&pendulum, nStepMax](double x, double y) -> int {
            return stepsToFlip(x, y, pendulum, nStepMax);
        }
    ));

    // Each cycle splits the region with highest priority in N new regions.
    for (int i = 0; i < nCycles; i++) {
        // Define the new regions.
        newRegions = (*std::prev(regions.end()))->getSubRegions();
        // Delete the old region.
        delete *std::prev(regions.end());
        regions.erase(std::prev(regions.end()));
        // Insert the new regions.
        for(auto newRegion = std::begin(newRegions); newRegion != std::end(newRegions); ++newRegion) {
            regions.insert(*newRegion);
        }
        // Every n cycles print the partial data collected up to now.
        // This can be used to leave the program running for an indefinite
        // amount of time and extract the results without having to interrupt it.
        if (nCyclesPrint >= 0 && i % nCyclesPrint == 0 && i > 0) {
            for(auto region = std::begin(regions); region != std::end(regions); ++region) {
                outFile << (*region)->getTextOutput();
            }
        }
    }

    // OUTPUT

    // Output the final data and clear the memory.
    for(auto region = std::begin(regions); region != std::end(regions); ++region) {
        outFile << (*region)->getTextOutput();
        delete *region;
    }
    regions.clear();

    auto chronoStop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(chronoStop - chronoStart);
    outFile << "# Elapsed time: " << duration.count() << " s";
}