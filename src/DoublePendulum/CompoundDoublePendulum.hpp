#ifndef COMPOUND_DOUBLE_PENDULUM
#define COMPOUND_DOUBLE_PENDULUM

#include "DoublePendulum.hpp"

/*
 * Implementation of a DoublePendulum where the masses are distribuited
 * uniformly along each rod.
 */
class CompoundDoublePendulum : public DoublePendulum {
    private:
        // Constants used in the equation of state.
        double c[5];

    public:
        CompoundDoublePendulum(double M1Val, double M2Val, double L1Val, double L2Val, double dtVal, double gVal);
        StateVector motionEquationStateForm(StateVector y);
        double getEnergy(StateVector state);
};

#endif