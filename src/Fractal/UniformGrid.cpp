#include <cmath>
#include <fstream>
#include <chrono>
#include <array>
#include <functional>
#include "UniformGrid.hpp"

UniformGrid::UniformGrid(std::shared_ptr<Fractal> fractal, int nStepMax, double ai1Min, double ai1Max, double ai2Min, double ai2Max, double gridSize) :
    fractal{fractal}, ai1Min{ai1Min}, ai1Max{ai1Max}, ai2Min{ai2Min}, ai2Max{ai2Max}, gridSize{gridSize}, nStepMax{nStepMax}
{
    this->imgSize[0] = (int) ceil((this->ai1Max - this->ai1Min) / this->gridSize);
    this->imgSize[1] = (int) ceil((this->ai2Max - this->ai2Min) / this->gridSize);
};

void UniformGrid::run(std::function<void(int col, int row, int stepCount)> f) {
    int row, col, stepCount;
    double ai1, ai2;

    row = 0;
    col = 0;
    // Loop until the (row, col) coordinates point outside the image.
    while (row < this->imgSize[1]) {
        // Convert (row, col) pixel position to (ai1, ai2) values.
        ai1 = this->ai1Min + col * this->gridSize;
        ai2 = this->ai2Min + row * this->gridSize;

        // Evaluate.
        stepCount = this->fractal->stepsToFlip(ai1, ai2, this->nStepMax);

        // Output.
        f(col, row, stepCount);
        
        // Step to the next pixel.
        col += 1;
        // If the pixel was out of the row switch to the next row.
        if (col >= this->imgSize[0]) {
            row += (int) (col / this->imgSize[0]);
            col = col % this->imgSize[0];
        }
    }
};

void UniformGrid::printDataToFile(const std::string fileName, const std::string separator) {
    std::ofstream outFile(fileName);
    std::string systemTypeStr;
    StateVector currState, nextState;

    auto chronoStart = std::chrono::high_resolution_clock::now();

    systemTypeStr = DoublePendulum::variantToString(this->fractal->pendulum->variant);

    // Write simulation parameters in the header.
    outFile << this->textComment << "M1" << "=" << this->fractal->pendulum->M1 << std::endl;
    outFile << this->textComment << "M2" << "=" << this->fractal->pendulum->M2 << std::endl;
    outFile << this->textComment << "L1" << "=" << this->fractal->pendulum->L1 << std::endl;
    outFile << this->textComment << "L2" << "=" << this->fractal->pendulum->L2 << std::endl;
    outFile << this->textComment << "type" << "=" << systemTypeStr << std::endl;

    outFile << this->textComment << "ai1Min" << "=" << this->ai1Min << std::endl;
    outFile << this->textComment << "ai1Max" << "=" << this->ai1Max << std::endl;
    outFile << this->textComment << "ai2Min" << "=" << this->ai2Min << std::endl;
    outFile << this->textComment << "ai2Max" << "=" << this->ai2Max << std::endl;

    outFile << this->textComment << "gridSize" << "=" << this->gridSize << std::endl;
    outFile << this->textComment << "dt" << "=" << this->fractal->pendulum->dt << std::endl;
    outFile << this->textComment << "g" << "=" << this->fractal->pendulum->g << std::endl;
    outFile << this->textComment << "nStepMax" << "=" << this->nStepMax << std::endl;
    
    outFile << this->textComment << "imgSizeX" << "=" << this->imgSize[0] << std::endl;
    outFile << this->textComment << "imgSizeY" << "=" << this->imgSize[1] << std::endl;
    
    outFile << this->textComment << "renderType" << "=" << "uniform" << std::endl;

    // Print the text output describing each pixel in outFile.
    this->run([this, separator, &outFile](int i, int j, int count) {
        outFile << i << separator << j << separator << count << std::endl;
    });

    // Write elapsed time in the footer.
    auto chronoStop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(chronoStop - chronoStart);
    outFile << "# Elapsed time: " << duration.count() << " s";
};