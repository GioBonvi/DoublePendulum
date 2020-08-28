#ifndef ADAPTIVE_GRID
#define ADAPTIVE_GRID

#include <memory>
#include <set>
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
        static const char textComment = '#';

        /**
         * Custom comparator to compare pointers of any type.
         * 
         * Note: the type T must be comparable itself.
         */
        class ComparePointers {
            public:
                template<typename T>
                bool operator()(T *a, T *b) {
                    return (*a) < (*b);
                }
        };
        // The multiset keeps the regions ordered by priority value, so
        // std::prev(regions.end()) always is the region with highest priority.
        // Note: a custom comparator is adopted to compare pointers.
        std::multiset<DataRegion*, ComparePointers> regions;

        void initRegions();

    public:
        AdaptiveGrid(std::shared_ptr<Fractal> fractal, int nStepMax, double ai1Central, double ai2Central, double aiSize);
        ~AdaptiveGrid();

        // Perform one or more calculation cycle.
        void cycle(int nCycles = 1);
        void printDataToFile(const std::string fileName, const std::string separator = "\t");
};

#endif