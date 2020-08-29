#ifndef ADAPTIVE_GRID
#define ADAPTIVE_GRID

#include <memory>
#include <set>
#include <png++/png.hpp>
#include "DataRegion.hpp"
#include "../Fractal.hpp"

/*
 * Sample the space with varying resolutions, depending on the complexity of
 * the data in every region.
 *
 * This is a more advanced sampling techinque than uniform grid, which employs
 * DataRegions to split the whole domain of the fractal in sub-regions each
 * of which receives a priority value based on how "complex" the data
 * distribution is expected to be inside it. More complex area will receive
 * a higher priority.
 * At each cycle the are with highest priority is split in smaller areas.
 * 
 * Following this strategy ensures that less resources are wasted computing
 * a high density of points in "flat" areas (e.g. the area at the center of
 * the fractal). 
 */
class AdaptiveGrid {
    private:
        const std::shared_ptr<Fractal> fractal;
        const double ai1Central, ai2Central, aiSize;
        // Maximum number of steps to solve the motion of the pendulum.
        const int nStepMax;
        // Text output lines starting with this character will be interpreted as comments, not data.
        static const char textComment;

        /**
         * Custom comparator to compare pointers of any type.
         * 
         * Note: the type T must be comparable itself.
         */
        class ComparePointers {
            public:
                template<typename T>
                bool operator()(std::unique_ptr<T> const &a, std::unique_ptr<T> const &b) {
                    return (*a) < (*b);
                }
        };
        // The multiset keeps the regions ordered by priority value, so
        // std::prev(regions.end()) always is the region with highest priority.
        // Note: a custom comparator is adopted to compare pointers.
        std::multiset<std::unique_ptr<DataRegion>, ComparePointers> regions;

        void initRegions();
        // Renders the data into a in-memory PNG image of the fractal.
        std::unique_ptr<png::image<png::rgb_pixel>> render();

    public:
        AdaptiveGrid(std::shared_ptr<Fractal> fractal, int nStepMax, double ai1Central, double ai2Central, double aiSize);
        ~AdaptiveGrid();

        // Perform one or more calculation cycles evaluating the fractal data and storing the results in memory.
        void cycle(int nCycles = 1);
        /*
         * Save the sampled data values in an ASCII file.
         * 
         * This file can be then read by other programs to render the image of
         * the fractal multiple times without having to perform the calculation
         * all over again.
         * 
         * The forceThreadNum parameter can be used to force a certain number
         * of threads to be used. If it is 0 the number of threads is automatically
         * assigned to be std::thread::hardware_concurrency().
         */
        void saveData(const std::string fileName, const std::string separator = "\t");
        // Save the image render of the fractal in a PNG file.
        void saveImage(const std::string fileName);
};

#endif