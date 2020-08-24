#ifndef SIMPLE_DOUBLE_PENDULUM
#define SIMPLE_DOUBLE_PENDULUM

#include "DoublePendulum.hpp"

/*
 * Implementation of a DoublePendulum where the masses are concentrated in the
 * second extremity of each rod. 
 */
class SimpleDoublePendulum : public DoublePendulum {

    public:
        SimpleDoublePendulum(double M1Val, double M2Val, double L1Val, double L2Val, double dtVal, double gVal);
        StateVector motionEquationStateForm(StateVector y);
        double getEnergy(StateVector state);
};

#endif