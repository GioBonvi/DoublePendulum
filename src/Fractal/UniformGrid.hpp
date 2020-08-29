#ifndef UNIFORM_GRID
#define UNIFORM_GRID

#include <memory>
#include <array>
#include <functional>
#include <mutex>
#include "Fractal.hpp"

/*
 * Simplest way to sample the values to draw the fractal: with a uniform grid.
 * 
 * The domain of the fractal is divided in a grid of squares of given size and
 * the target function is evaluated at the vertices of these squares.
 */
class UniformGrid {
    private:
        const std::shared_ptr<Fractal> fractal;
        // Domain of the fractal.
        const double ai1Min, ai1Max, ai2Min, ai2Max;
        // Resolution of the grid on which the values are evalutated.
        const double gridSize;
        // Maximum number of steps to solve the motion of the pendulum.
        const int nStepMax;
        // Final image size [x, y].
        std::array<int, 2> imgSize;
        // Text output lines starting with this character will be interpreted as comments, not data.
        static const char textComment = '#';
        // For thread safe output to a file.
        std::mutex outFileMutex;

        /*
         * Loop over all the pixels and for each one calculate the fractal
         * value, then evaluate f(col, row, stepCount).
         * 
         * This can be used to implement different output types (e.g. text
         * dump of the data, build an in-memory image...)
         */
        void run(std::function<void(int col, int row, int stepCount)> f);
        /*
         * This is the same as run(), but only operates on a subset of pixels.
         * 
         * Each thread, thorugh the threadsNum and threadIndex arguments, is
         * assigned a different, non-intersecting set of pixels to calculate
         * autonomously.
         */
        void runThreaded(int threadsNum, int threadIndex, std::function<void(int col, int row, int stepCount)> f);

    public:
        UniformGrid(std::shared_ptr<Fractal> fractal, int nStepMax,
                    double ai1Min, double ai1Max, double ai2Min, double ai2Max, double gridSize);

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
        void saveData(const std::string fileName, const std::string separator = "\t", int forceThreadNum = 0);
};

#endif