#ifndef DOUBLE_PENDULUM
#define DOUBLE_PENDULUM

#include <memory>
#include <string>
#include <array>
#include "StateVector.hpp"

/*
 * Abstract class describing a generic double pendulum system, composed by two
 * rods: the first connected to the ground in the origin O with a pin in the
 * first extremity and connected with antoher pin in its second extremity to
 * the first extremity of the second rod.
 * 
 * The distribution of the masses on the rods is not defined in this class, but
 * in its derived classes: SimpleDoublePendulum and CompoundDoublePendulum.
 * 
 * This class defines various base methods: most importantly calcNextState(),
 * in which the equation of motion of the system is solved numerically to get
 * the next state of the system. Other methods specify some info on the system
 * based on the current state.
 */
class DoublePendulum {
    public:
        // Pendulum variants.
        enum class Variant {Simple, Compound};
        static std::string variantToString(DoublePendulum::Variant pendulumType);

        // Physical parameters of the system.
        const double M1, M2, L1, L2;
        const Variant variant;
        const double dt, g;

        // 2 * 2 degrees of freedom.
        static const int N_STATE_VARS = 2 * 2;
        // x and y for 5 points (O, G1, A, G2, B)
        static const int N_COORDS = 2 * 5;

        // Factory method which uses the correct class to instantiate a new DoublePendulum based of the correct Variant.
        static std::unique_ptr<DoublePendulum> makeDoublePendulum(double M1, double M2, double L1, double L2, double dt, double g, Variant variant);

        DoublePendulum(double M1, double M2, double L1, double L2, double dt, double g, Variant variant);

        // State equation of the pendulum: out = f(y)
        virtual StateVector motionEquationStateForm(StateVector y) = 0;
        StateVector calcNextState(StateVector currState);
        // Get the values of position, velocity and energy of the various elements of the system at a given state.
        std::array<double, N_COORDS> getCartesianCoordinates(StateVector state);
        std::array<double, N_COORDS> getCartesianVelocities(StateVector state);
        virtual double getEnergy(StateVector state) = 0;
        // Get the values characterizing a state in text form.
        std::string getTextOutput(StateVector state, const std::string &separator="\t");        
};

#endif