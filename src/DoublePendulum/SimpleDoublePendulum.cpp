#include <cmath>
#include "SimpleDoublePendulum.hpp"

SimpleDoublePendulum::SimpleDoublePendulum(double M1, double M2, double L1, double L2, double dt, double g) :
    DoublePendulum(M1, M2, L1, L2, dt, g, DoublePendulum::Variant::Simple) {};

/*
 * Equations of motion of a simple double pendulum in state form.
 * 
 * Source: http://www.physics.usyd.edu.au/~wheat/dpend_html/
 */
StateVector SimpleDoublePendulum::motionEquationStateForm(StateVector y) {
    StateVector out;

    out[0] = y.w1;
    out[1] = (
        this->M2 * this->L1 * cos(y.a2 - y.a1) * sin(y.a2 - y.a1) * pow(y.w1, 2)
        + this->M2 * this->L2 * sin(y.a2 - y.a1) * pow(y.w2, 2)
        - (this->M1 + this->M2) * this->g * sin(y.a1)
        + this->M2 * this->g * cos(y.a2 - y.a1) * sin(y.a2)
    ) / (
        (this->M1 + this->M2) * this->L1 - this->M2 * this->L1 * pow(cos(y.a2 - y.a1), 2)
    );
    out[2] = y.w2;
    out[3] = (
        - (this->M1 + this->M2) * this->L1 * sin(y.a2 - y.a1) * pow(y.w1, 2)
        - this->M2 * this->L2 * cos(y.a2 - y.a1) * sin(y.a2 - y.a1) * pow(y.w2, 2)
        + (this->M1 + this->M2) * this->g * cos(y.a2 - y.a1) * sin(y.a1)
        - (this->M1 + this->M2) * this->g * sin(y.a2)
    ) / (
        (this->M1 + this->M2) * this->L2 - this->M2 * this->L2 * pow(cos(y.a2 - y.a1), 2)
    );

    return out;
};

double SimpleDoublePendulum::getEnergy(StateVector state) {
    std::array<double, N_COORDS> coords;
    std::array<double, N_COORDS> vel;
    double energy;
    
    coords = this->getCartesianCoordinates(state);
    vel = this->getCartesianVelocities(state);

    // NOTE: For the rod the center of mass coincides with the second
    //       extremity, where the mass is placed.
    //       Since the mass is 1D there is no moment of inertia.
    
    // Base energy so that E_tot = 0 if the pendulum is vertical and still.
    energy = this->M1 * this->g * this->L1 + this->M2 * this->g * this->L2;
    // Potential gravitational.
    energy += - this->M1 * this->g * coords[5] - this->M2 * this->g * coords[9];
    // Kinetic translational.
    energy += this->M1 * (pow(vel[4], 2) + pow(vel[5], 2)) / 2
           +  this->M2 * (pow(vel[8], 2) + pow(vel[9], 2)) / 2;
    
    return energy;
}