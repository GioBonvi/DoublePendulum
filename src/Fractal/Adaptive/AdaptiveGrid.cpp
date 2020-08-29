#include <array>
#include <fstream>
#include "DataRegion.hpp"
#include "AdaptiveGrid.hpp"

const char AdaptiveGrid::textComment = '#';

AdaptiveGrid::AdaptiveGrid(std::shared_ptr<Fractal> fractal, int nStepMax, double ai1Central, double ai2Central, double aiSize) :
    fractal{fractal}, ai1Central{ai1Central}, ai2Central{ai2Central}, aiSize{aiSize}, nStepMax{nStepMax} {
        this->initRegions();
    };

AdaptiveGrid::~AdaptiveGrid() {
    for(auto region = std::begin(this->regions); region != std::end(this->regions); ++region) {
        delete *region;
    }
    regions.clear();
};

void AdaptiveGrid::initRegions() {
    // The first region covers the whole domanin: subregions will be defined
    // automatically around the most "interesting" areas.
    this->regions.insert(new DataRegion(
        this->ai1Central,
        this->ai2Central,
        this->aiSize,
        this->aiSize,
        // Lambda expression to fit the f(x, y) format required by DataRegion.
        [this](double x, double y) -> int {
            return this->fractal->stepsToFlip(x, y, this->nStepMax);
        }
    ));
};

void AdaptiveGrid::cycle(int nCycles) {
    std::array<DataRegion*, DataRegion::DATA_POINTS_N> newRegions;

    for (int i = 0; i < nCycles; i++) {
        // Define the new regions based on the highest priority region.
        newRegions = (*std::prev(regions.end()))->getSubRegions();

        // Delete the old region.
        delete *std::prev(regions.end());
        regions.erase(std::prev(regions.end()));

        // Insert the new regions.
        for(auto newRegion = std::begin(newRegions); newRegion != std::end(newRegions); ++newRegion) {
            regions.insert(*newRegion);
        }
    }
}

void AdaptiveGrid::saveData(const std::string fileName, const std::string separator) {
    std::ofstream outFile(fileName);
    std::string systemTypeStr;
    StateVector currState, nextState;

    systemTypeStr = DoublePendulum::variantToString(this->fractal->pendulum->variant);

    // Write simulation parameters in the header.
    outFile << this->textComment << "M1" << "=" << this->fractal->pendulum->M1 << std::endl;
    outFile << this->textComment << "M2" << "=" << this->fractal->pendulum->M2 << std::endl;
    outFile << this->textComment << "L1" << "=" << this->fractal->pendulum->L1 << std::endl;
    outFile << this->textComment << "L2" << "=" << this->fractal->pendulum->L2 << std::endl;
    outFile << this->textComment << "type" << "=" << systemTypeStr << std::endl;

    outFile << this->textComment << "ai1Central" << "=" << this->ai1Central << std::endl;
    outFile << this->textComment << "ai2Central" << "=" << this->ai2Central << std::endl;
    outFile << this->textComment << "aiSize" << "=" << this->aiSize << std::endl;

    outFile << this->textComment << "dt" << "=" << this->fractal->pendulum->dt << std::endl;
    outFile << this->textComment << "g" << "=" << this->fractal->pendulum->g << std::endl;
    outFile << this->textComment << "nStepMax" << "=" << this->nStepMax << std::endl;
    outFile << this->textComment << "nCycles" << "=" << this->nStepMax << std::endl;
    
    outFile << this->textComment << "renderType" << "=" << "adaptive" << std::endl;
    
    for(auto region = std::begin(regions); region != std::end(regions); ++region) {
        outFile << (*region)->getTextOutput();
    }

};