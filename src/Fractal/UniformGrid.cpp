#include <memory>
#include <cmath>
#include <fstream>
#include <vector>
#include <thread>
#include <png++/image.hpp>
#include <png++/rgb_pixel.hpp>
#include "UniformGrid.hpp"
#include "ColorScale.hpp"

const char UniformGrid::textComment = '#';

UniformGrid::UniformGrid(std::shared_ptr<Fractal> fractal, int nStepMax, double ai1Min, double ai1Max, double ai2Min, double ai2Max, double gridSize) :
    fractal{fractal}, ai1Min{ai1Min}, ai1Max{ai1Max}, ai2Min{ai2Min}, ai2Max{ai2Max}, gridSize{gridSize}, nStepMax{nStepMax}
{
    this->imgSize[0] = (int) ceil((this->ai1Max - this->ai1Min) / this->gridSize);
    this->imgSize[1] = (int) ceil((this->ai2Max - this->ai2Min) / this->gridSize);

    data.resize(imgSize[0] * imgSize[1], Fractal::STEPS_OUT_OF_SCALE);
};

void UniformGrid::calcThreaded(int threadsNum, int threadIndex) {
    // Pixel coordinates in the image pixel reference system (origin top left,
    // x positive to the right, y positive to the bottom).
    int img_x, img_y;
    // Initial conditions in the user reference system (origin in the center, x
    // positive to the right, y positive to the top)
    double ai1, ai2;

    // Each thread starts at an offset so as not to overlap with other threads.
    img_y = 0;
    img_x = threadIndex;

    // Check that we are still inside the image boundaries.
    if (img_x >= this->imgSize[0]) {
        img_y += (int) (img_x / this->imgSize[0]);
        img_x = img_x % this->imgSize[0];
    }
    // Loop until the (img_x, img_y) coordinates point outside the image.
    while (img_y < this->imgSize[1]) {
        // Convert (img_x, img_y) pixel position to (ai1, ai2) values.
        // NOTE: Image and user coordinate systems have inverted y axis.
        ai1 = this->ai1Min + img_x * this->gridSize;
        ai2 = this->ai2Max - img_y * this->gridSize;

        // Evaluate.
        this->data[img_y * this->imgSize[0] + img_x] = this->fractal->stepsToFlip(ai1, ai2, this->nStepMax);
        
        // Step to the next pixel, skipping all the pixels other threads are working on.
        img_x += threadsNum;
        // If the pixel was out of the row switch to the next row.
        if (img_x >= this->imgSize[0]) {
            img_y += (int) (img_x / this->imgSize[0]);
            img_x = img_x % this->imgSize[0];
        }
    }
};

void UniformGrid::calcData(int forceThreadNum) {
    // Multiple threads can be used to calculate the pixel data in parallel.
    int nThreads;
    if (forceThreadNum == 0) {
        nThreads = std::thread::hardware_concurrency();
    } else {
        nThreads = forceThreadNum;
    }
    std::vector<std::thread> threads;

    // Create N-1 new threds since the main which is already in execution
    // is one of the N threads.
    for (int i = 0; i < nThreads - 1; i++) {
        threads.push_back(std::thread(&UniformGrid::calcThreaded, this, nThreads, i));
    }
    // No need for std::thread() to execute code on the main thread.
    this->calcThreaded(nThreads, nThreads - 1);

    // Wait for all the threads to finish.
    for (auto &t: threads) {
        t.join();
    }
}

void UniformGrid::saveData(const std::string fileName, const std::string separator) {
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

    // Output data.
    int x, y;
    for (uint i = 0; i < this->data.size(); i++) {
        x = i % imgSize[0];
        y = i / imgSize[0];
        outFile << x << separator << y << separator << this->data[i] << std::endl;
    }
};

std::unique_ptr<png::image<png::rgb_pixel>> UniformGrid::render() {
    auto img = std::make_unique<png::image<png::rgb_pixel>>(this->imgSize[0], this->imgSize[1]);
    ColorScale colorScale = ColorScale();
    float baseSteps = sqrt(this->fractal->pendulum->L1 / this->fractal->pendulum->g) / this->fractal->pendulum->dt;
    
    // Output data.
    int x, y;
    for (uint i = 0; i < this->data.size(); i++) {
        x = i % imgSize[0];
        y = i / imgSize[0];
        img->set_pixel(x, y, colorScale.getColor(data[this->imgSize[0] * y + x] / baseSteps, Fractal::STEPS_OUT_OF_SCALE));
    }
    
    return img;
};

void UniformGrid::saveImage(const std::string fileName) {
    auto img = this->render();
    img->write(fileName);
}