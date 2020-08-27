#ifndef DATA_REGION
#define DATA_REGION

#include <string>
#include <memory>
#include <functional>
#include "DataPoint.hpp"

/*
 * A DataRegion is a collection of N DataPoints and is used to evaluate a
 * function f(x, y) over its domain with variable discretization in order to
 * increase the discretiziation density where the function is more chaotic and
 * to decrease it where it behaves more regularly.
 *
 * This is useful, for example, in rendering a fractal generated by iterating a
 * process up to a maximum number of steps: in these types of fractals there
 * are various zones where the calculation would proceed to a very large number
 * (or even to infinity) which consume a lot of cycles while not producing very
 * interesting results.
 * 
 * A DataRegion divides its (square) domain in N (3x3=9 by default)
 * sub-regions, evaluating a DataPoint with the function f(x, y) in the center
 * of each one.
 * Then a priority value is calculated for the region considering the side
 * length of its subregions (smaller regions have already been sampled better,
 * so have lower priority) and the coefficient of variation of its N DataPoints
 * (regions with a lower coefficient have a lower priority since they probably
 * are more uniform).
 */
class DataRegion {
    public:
        /*
         * Must be an odd number greater than 1.
         * Odd in order to keep the already evaluated DataPoints as center
         * values for the subregions.
         * If it was one there would be no increase in the sampling density.
         */
        static const int DATA_POINTS_ON_1D = 3;
        static const int DATA_POINTS_N = DATA_POINTS_ON_1D * DATA_POINTS_ON_1D;
        double priority;

        /*
         * The DataRegion covers a square area of length size, whose center
         * has coordiantes (x, y).
         * f(x, y) is the function defined on the whole xy domain.
         */
        DataRegion(double x, double y, double size, double fullDomainSize, std::function<double(double, double)> f);
        /*
         * If the value corresponding to the central node is already known it
         * can be passed directly, avoinding one evaluation of f(x, y).
         */
        DataRegion(double x, double y, double size, double fullDomainSize, std::function<double(double, double)> f, double centralValue);
        /*
         * DataRegion can be directly initialized providing the DataPoint which
         * was located in the central subregion of the previously existing
         * sub-region.
         */
        DataRegion(DataPoint centralDp, double fullDomainSize, std::function<double(double, double)> f);

        // Generates the new regions from the existing subregions.
        std::array<DataRegion*, DATA_POINTS_N> getSubRegions(int forceThreadNum = 0);
        
        // Text output passed to a Python script for image rendering.
        std::string getTextOutput(const char *separator = "\t");

        // Two DataRegions can be confronted directly through their priority value.
        friend bool operator< (const DataRegion &dp1, const DataRegion &dp2);
        friend bool operator<= (const DataRegion &dp1, const DataRegion &dp2);
        friend bool operator> (const DataRegion &dp1, const DataRegion &dp2);
        friend bool operator>= (const DataRegion &dp1, const DataRegion &dp2);      

    private:
        // The function to be evaluated is passed to each subregion when it is created.
        std::function<double(double, double)> f;
        double fullDomainSize;
        DataPoint dataPoints[DATA_POINTS_N];
        // The algorithm to calculate the priority value of the region.
        void calcPriority();
};

#endif