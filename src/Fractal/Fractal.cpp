#define _USE_MATH_DEFINES
#include <memory>
#include <cmath>
#include "Fractal.hpp"
#include "../DoublePendulum/SimpleDoublePendulum.hpp"
#include "../DoublePendulum/CompoundDoublePendulum.hpp"

Fractal::Fractal(std::unique_ptr<DoublePendulum> pendulum) :
    pendulum{std::move(pendulum)} {};

// Copy operator.
Fractal& Fractal::operator=(Fractal &&f) {
    if (this != &f)
    {
        this->pendulum = std::move(f.pendulum);
    }
    return *this;
};

// Move constructor.
Fractal::Fractal(Fractal &&f) : pendulum(std::move(f.pendulum)) {}

bool Fractal::detectFlip(StateVector prevState, StateVector currState) {
    // The offset by PI is to start counting rounds at the top (at an agle of PI radians
    // in the global reference system) instead of at the bottom (0 radians).
    float nRoundsRod1PrevState = floor((prevState.a1 - M_PI) / (2 * M_PI));
    float nRoundsRod1CurrState = floor((currState.a1 - M_PI) / (2 * M_PI));
    float nRoundsRod2PrevState = floor((prevState.a2 - M_PI) / (2 * M_PI));
    float nRoundsRod2CurrState = floor((currState.a2 - M_PI) / (2 * M_PI));

    return (nRoundsRod1PrevState != nRoundsRod1CurrState) || (nRoundsRod2PrevState != nRoundsRod2CurrState);
};


int Fractal::stepsToFlip(double ai1, double ai2, int nStepMax) {
    int count;
    StateVector currState, nextState;
    
    // Initial state.
    currState.a1 = ai1;
    currState.w1 = 0;
    currState.a2 = ai2;
    currState.w2 = 0;

    // If this condition is not met then it is physically impossible for any rod to flip.
    // See: http://csaapt.org/uploads/3/4/4/2/34425343/csaapt_maypalace_sp16.pdf
    if (3 * this->pendulum->L1 * cos(currState[0]) + this->pendulum->L2 * cos(currState[2]) > 2) {
        return nStepMax;
    }

    // Numerically solve the state equation.
    for (count = 0; count < nStepMax; count++) {
        nextState = this->pendulum->calcNextState(currState);

        // Check if a flip happened between the last two states.
        if (count > 1 && this->detectFlip(currState, nextState)) {
            break;
        }

        // Update the current state.
        currState = nextState;
    }
    return count;
};