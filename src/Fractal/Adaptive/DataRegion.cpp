#include <cmath>
#include <string>
#include <sstream>
#include <functional>
#include <memory>
#include <array>
#include <thread>
#include <vector>
#include "DataRegion.hpp"
#include "DataPoint.hpp"

DataRegion::DataRegion(DataPoint dp, double fullDomainSize, std::function<double(double, double)> f) :
    DataRegion(dp.x, dp.y, dp.size, fullDomainSize, f, dp.val) {};

DataRegion::DataRegion(double x, double y, double size, double fullDomainSize, std::function<double(double, double)> f) :
    DataRegion(x, y, size, fullDomainSize, f, f(x, y)) {};

DataRegion::DataRegion(double x, double y, double size, double fullDomainSize, std::function<double(double, double)> f, double centralValue) {
    double segmentSize, newValue;
    double xDataPoint, yDataPoint;
    int i, j, minIndex;

    this->f = f;
    this->fullDomainSize = fullDomainSize;
    
    segmentSize = size / DATA_POINTS_ON_1D;

    /*
     * Divide the DataRegion in a grid of DATA_POINTS_ON_1D x DATA_POINTS_ON_1D
     * squares: at the center of each square evaluate a DataPoint with f(x, y).
     * 
     * The central square is identified with indices (0, 0) and all the other square
     * as (i, j) with positive i to the right, positive j to the left.
     */
    minIndex = (int) (DATA_POINTS_ON_1D / 2);
    for (i = -minIndex; i <= minIndex; i++) {
        xDataPoint = x + i * segmentSize;

        for (j = -minIndex; j <= minIndex; j++) {
            yDataPoint = y + j * segmentSize;

            if (i == 0 && j == 0) {
                // Use the already calculated value for the central grid, skipping one calculation.
                newValue = centralValue;
            } else {
                // Calculate the value of f(x, y) for any other grid.
                newValue = f(xDataPoint, yDataPoint);
            }

            dataPoints[(i + minIndex) * DATA_POINTS_ON_1D + (j + minIndex)].update(xDataPoint, yDataPoint, newValue, segmentSize);
        }
    }

    calcPriority();
}

std::array<std::unique_ptr<DataRegion>, DataRegion::DATA_POINTS_N> DataRegion::getSubRegions(int forceThreadNum) {
    std::array<std::unique_ptr<DataRegion>, DATA_POINTS_N> subRegions;
    int threadsNum;
    std::vector<std::thread> threads;

    // Multiple threads can be used to calculate the pixel data in parallel.
    if (forceThreadNum == 0) {
        threadsNum = std::thread::hardware_concurrency();
    } else {
        threadsNum = forceThreadNum;
    }

    auto createRegions = [&subRegions, this](int threadsNum, int threadIndex) {
        // Each thread creates different regions thanks to the different offset.
        for (int i = threadIndex; i < DATA_POINTS_N; i += threadsNum) {
            subRegions[i] = std::make_unique<DataRegion>(this->dataPoints[i], this->fullDomainSize, this->f);
        }
    };

    // Create N-1 threads...
    for (int threadIndex = 0; threadIndex < threadsNum - 1; threadIndex++) {
        threads.push_back(std::thread(createRegions, threadsNum, threadIndex));
    }
    // ... and also use the current thread.
    createRegions(threadsNum, threadsNum-1);

    // Wait for all threads to finish.
    for (auto &t: threads) {
        t.join();
    }

    return subRegions;
}

std::string DataRegion::getTextOutput(const char *separator) {
    std::stringstream ss;

    for(auto dp = std::begin(dataPoints); dp != std::end(dataPoints); ++dp) {
        ss << (*dp).x << separator  << (*dp).y << separator << (*dp).size << separator << (*dp).val << std::endl;
    }
    return ss.str();
}

// Comparison is based on priority values.
bool operator< (const DataRegion &dp1, const DataRegion &dp2) {
    return dp1.priority < dp2.priority;
}

bool operator<= (const DataRegion &dp1, const DataRegion &dp2) {
    return dp1.priority <= dp2.priority;
}

bool operator> (const DataRegion &dp1, const DataRegion &dp2) {
    return dp1.priority > dp2.priority;
}

bool operator>= (const DataRegion &dp1, const DataRegion &dp2) {
    return dp1.priority >= dp2.priority;
}

/*
 * Priority is directly proportianal to the side length of the subregions
 * and to the coefficient of variation of the DataPoints.
 */
void DataRegion::calcPriority() {
    int i;
    double mean, sigma, cv;

    mean = 0;
    for (i = 0; i < DATA_POINTS_N; i++) {
        mean += dataPoints[i].val;
    }
    mean = mean / DATA_POINTS_N;

    sigma = 0;
    for (i = 0; i < DATA_POINTS_N; i++) {
        sigma = sigma + pow(dataPoints[i].val - mean, 2);
    }
    sigma = sqrt(sigma);

    // Unbiased formulation of CV.
    // Source: https://en.wikipedia.org/wiki/Coefficient_of_variation#Estimation
    cv = (1  + 0.25 / DATA_POINTS_N) * sigma / mean;

    /* 
     * This is a key element of this class and might need to be optimized or
     * adjusted.
     * Up to now it just considers size and the coefficient of variation in a
     * simple direct proportionality. Some factors might be added to weigh one
     * with respect to the other, for example by adding an exponent to each member.
     */
    priority = pow((1 + cv), 2)  * dataPoints[0].size / fullDomainSize;
}