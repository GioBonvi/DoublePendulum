#include <cmath>
#include <array>
#include <fstream>
#include "DataRegion.hpp"
#include "AdaptiveGrid.hpp"
#include "../ColorScale.hpp"

const char AdaptiveGrid::textComment = '#';

AdaptiveGrid::AdaptiveGrid(std::shared_ptr<Fractal> fractal, int nStepMax, double ai1Central, double ai2Central, double aiSize) :
    fractal{fractal}, ai1Central{ai1Central}, ai2Central{ai2Central}, aiSize{aiSize}, nStepMax{nStepMax} {
        this->initRegions();
    };

AdaptiveGrid::~AdaptiveGrid() {
    regions.clear();
};

void AdaptiveGrid::initRegions() {
    // The first region covers the whole domanin: subregions will be defined
    // automatically around the most "interesting" areas.
    this->regions.insert(std::make_unique<DataRegion>(
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

std::unique_ptr<png::image<png::rgb_pixel>> AdaptiveGrid::render() {
    double minSize, size;
    struct { int x; int y; } imgSize;
    ColorScale colorScale = ColorScale();
    float baseSteps;
    
    minSize = this->aiSize;
    // Identify the resolution of the image by finding the minimum subregion side length.
    for (auto &region: this->regions) {
        size = region->dataPoints[0].size;
        if (size < minSize) {
            minSize = size;
        }
    }
    imgSize.x = round(this->aiSize / minSize);
    imgSize.y = imgSize.x;

    // Initialize the image.
    auto img = std::make_unique<png::image<png::rgb_pixel>>(imgSize.x, imgSize.y);

    auto drawSquare = [&img](int xCenter, int yCenter, int sizeLen, png::rgb_pixel color) {
        int halfSizeLen = sizeLen / 2;
        for (int x = xCenter - halfSizeLen; x <= xCenter + halfSizeLen; x++) {
            for (int y = yCenter - halfSizeLen; y <= yCenter + halfSizeLen; y++) {
                img->set_pixel(x, y, color);
            }
        }
    };

    // Calculate all the pixels.
    baseSteps = sqrt(this->fractal->pendulum->L1 / this->fractal->pendulum->g) / this->fractal->pendulum->dt;
    int x, y;
    for (auto &region: this->regions) {
        for (auto &dp: region->dataPoints) {
            x = (int) ((dp.x + this->aiSize / 2) / minSize);
            y = (int) ((dp.y + this->aiSize / 2) / minSize);
            drawSquare(x, y, region->dataPoints[0].size / minSize, colorScale.getColor(dp.val / baseSteps, Fractal::STEPS_OUT_OF_SCALE));
        }
    }

    return img;
};

void AdaptiveGrid::cycle(int nCycles) {
    std::array<std::unique_ptr<DataRegion>, DataRegion::DATA_POINTS_N> newRegions;

    for (int i = 0; i < nCycles; i++) {
        // Define the new regions based on the highest priority region.
        newRegions = (*(this->regions.rbegin()))->getSubRegions();

        regions.erase(std::prev(this->regions.end()));
        
        // Insert the new regions.
        for(auto newRegion = std::begin(newRegions); newRegion != std::end(newRegions); ++newRegion) {
            regions.insert(std::move(*newRegion));
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

void AdaptiveGrid::saveImage(const std::string fileName) {
    auto img = this->render();
    img->write(fileName);
};