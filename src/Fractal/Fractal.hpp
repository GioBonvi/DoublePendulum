#ifndef FRACTAL
#define FRACTAL

#include <memory>
#include "../DoublePendulum/DoublePendulum.hpp"
#include "../DoublePendulum/StateVector.hpp"

/*
 * It is possible to draw a fractal by evaluating after how much time a double
 * pendulum system has one of its rod flip vertically while varying its initial
 * state.
 * 
 * Source: https://www.famaf.unc.edu.ar/~vmarconi/fiscomp/Double.pdf
 * 
 * This class provides some functions to calulate this, while referring to a
 * DoublePendulum object.
 */
class Fractal {
    public:
        static const int STEPS_OUT_OF_SCALE;
        // Pointer to the double pendulum to observe.
        std::unique_ptr<DoublePendulum> pendulum;

        Fractal(std::unique_ptr<DoublePendulum> pendulum);

        // Copy operator.
        Fractal& operator=(Fractal &&f);
        // Move constructor.
        Fractal(Fractal &&f);

        /*
         * Detect wether one of the two rods flipped between the two states.
         * 
         * A rod flips when it passes through the vertical upwards position
         * (alpha = +/- pi) clockwise or anticlockwise.
         * This can be detected by counting the number of complete circles made
         * by each rod: if it differs between the two states then the rod
         * flipped between those states.
         */
        static bool detectFlip(StateVector prevState, StateVector currState);
        /*
         * Count how many steps it takes for the pendulum to "flip" from the
         * given initial condition.
         */
        int stepsToFlip(double ai1, double ai2, int nStepMax);

};

#endif