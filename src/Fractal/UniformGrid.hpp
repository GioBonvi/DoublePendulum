#ifndef UNIFORM_GRID
#define UNIFORM_GRID

#include <memory>
#include <array>
#include <functional>
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

        /*
         * Loop over all the pixels and for each one calculate the fractal
         * value, then evaluate f(col, row, stepCount).
         * 
         * This can be used to implement different output types (e.g. text
         * dump of the data, build an in-memory image...)
         */
        void run(std::function<void(int col, int row, int stepCount)> f);

    public:
        UniformGrid(std::shared_ptr<Fractal> fractal, int nStepMax,
                    double ai1Min, double ai1Max, double ai2Min, double ai2Max, double gridSize);

        /*
         * Dump the sampled data values in an ASCII file.
         * 
         * This file can be then read by other programs (e.g. fractal_render.py)
         * to render the image of the fractal.
         */
        void printDataToFile(const std::string fileName, const std::string separator = "\t");
};

#endif