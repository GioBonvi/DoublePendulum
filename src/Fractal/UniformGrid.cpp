#include <cmath>
#include <fstream>
#include <chrono>
#include <array>
#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include "UniformGrid.hpp"

UniformGrid::UniformGrid(std::shared_ptr<Fractal> fractal, int nStepMax, double ai1Min, double ai1Max, double ai2Min, double ai2Max, double gridSize) :
    fractal{fractal}, ai1Min{ai1Min}, ai1Max{ai1Max}, ai2Min{ai2Min}, ai2Max{ai2Max}, gridSize{gridSize}, nStepMax{nStepMax}
{
    this->imgSize[0] = (int) ceil((this->ai1Max - this->ai1Min) / this->gridSize);
    this->imgSize[1] = (int) ceil((this->ai2Max - this->ai2Min) / this->gridSize);
};

void UniformGrid::runThreaded(int threadsNum, int threadIndex, std::function<void(int col, int row, int stepCount)> f) {
    int row, col, stepCount;
    double ai1, ai2;

    // Each thread starts at an offset so as not to overlap with other threads.
    row = 0;
    col = threadIndex;

    // Check that we are still inside the image boundaries.
    if (col >= this->imgSize[0]) {
        row += (int) (col / this->imgSize[0]);
        col = col % this->imgSize[0];
    }
    // Loop until the (row, col) coordinates point outside the image.
    while (row < this->imgSize[1]) {
        // Convert (row, col) pixel position to (ai1, ai2) values.
        ai1 = this->ai1Min + col * this->gridSize;
        ai2 = this->ai2Max - row * this->gridSize; // Image coordinate system has y axis inverted.

        // Evaluate.
        stepCount = this->fractal->stepsToFlip(ai1, ai2, this->nStepMax);

        // Output.
        f(col, row, stepCount);
        
        // Step to the next pixel, skipping all the pixels other threads are working on.
        col += threadsNum;
        // If the pixel was out of the row switch to the next row.
        if (col >= this->imgSize[0]) {
            row += (int) (col / this->imgSize[0]);
            col = col % this->imgSize[0];
        }
    }
};

void UniformGrid::run(std::function<void(int col, int row, int stepCount)> f) {
    // Run thread with a single thread.
    this->runThreaded(1, 0, f);
};

void UniformGrid::saveData(const std::string fileName, const std::string separator, int forceThreadNum) {
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

    // Multiple threads can be used to calculate the pixel data in parallel.
    int nThreads;
    if (forceThreadNum == 0) {
        nThreads = std::thread::hardware_concurrency();
    } else {
        nThreads = forceThreadNum;
    }
    std::vector<std::thread> threads;

    // Thread safe print data thanks to the mutex.
    auto printData = [this, separator, &outFile](int i, int j, int stepCount) {
        const std::lock_guard<std::mutex> lock(this->outFileMutex);
        outFile << i << separator << j << separator << stepCount << std::endl;
    };

    // Create N-1 new threds since the main which is already in execution
    // is one of the N threads.
    for (int i = 0; i < nThreads - 1; i++) {
        threads.push_back(std::thread(&UniformGrid::runThreaded, this, nThreads, i, printData));
    }
    // No need for std::thread() to execute code on the main thread.
    this->runThreaded(nThreads, nThreads - 1, printData);

    // Wait for all the threads to finish.
    for (auto &t: threads) {
        t.join();
    }

    // Write elapsed time in the footer.
    auto chronoStop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(chronoStop - chronoStart);
    outFile << "# Elapsed time: " << duration.count() << " s";
};