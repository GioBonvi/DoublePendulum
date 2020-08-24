#include <cmath>
#include "CompoundDoublePendulum.hpp"

CompoundDoublePendulum::CompoundDoublePendulum(double M1, double M2, double L1, double L2, double dt, double g) :
    DoublePendulum(M1, M2, L1, L2, dt, g) {
    
    // Constants used in the equation of state.
    this->c[0] = this->M1 * pow(this->L1 / 2.0, 2) / 2.0
               + this->M1 * pow(this->L1, 2) / 12.0 / 2.0
               + this->M2 * pow(this->L1, 2) / 2.0;
    this->c[1] = this->M2 * pow(this->L2 / 2.0, 2) / 2.0
               + this->M2 * pow(this->L2, 2) / 12.0 / 2.0;
    this->c[2] = this->M2 * this->L1 * this->L2 / 2.0;
    this->c[3] = this->g * (this->M1 * this->L1 / 2.0 + this->M2 * this->L1);
    this->c[4] = this->g * this->M2 * this->L2 / 2.0;
};

/*
 * Equations of motion of a compound double pendulum in state form.
 * 
 * Source: https://www.astro.umd.edu/~adhabal/V1/Reports/Order_and_Chaos.pdf
 */
StateVector CompoundDoublePendulum::motionEquationStateForm(StateVector y) {
    StateVector out;

    out[0] = y.w1;
    out[1] = (
        2 * this->c[1] * this->c[3] * sin(y.a1)
        + pow(this->c[2], 2) * pow(y.w1, 2) * sin(y.a1 - y.a2) * cos(y.a1 - y.a2)
        + 2 * this->c[1] * this->c[2] * pow(y.w2, 2) * sin(y.a1 - y.a2)
        - this->c[2] * this->c[4] * cos(y.a1 - y.a2) * sin(y.a2)
    ) / (
        pow(this->c[2], 2) * pow(cos(y.a1 - y.a2), 2) - 4 * this->c[0] * this->c[1]
    );
    out[2] = y.w2;
    out[3] = (
        2 * this->c[0] * this->c[4] * sin(y.a2)
        - pow(this->c[2], 2) * pow(y.w2, 2) * sin(y.a1 - y.a2) * cos(y.a1 - y.a2)
        - 2 * this->c[0] * this->c[2] * pow(y.w1, 2) * sin(y.a1 - y.a2)
        - this->c[2] * this->c[3] * cos(y.a1 - y.a2) * sin(y.a1)
    ) / (
        pow(this->c[2], 2) * pow(cos(y.a1 - y.a2), 2) - 4 * this->c[0] * this->c[1]
    );

    return out;
};


double CompoundDoublePendulum::getEnergy(StateVector state) {
    std::array<double, N_COORDS> coords;
    std::array<double, N_COORDS> vel;
    double energy;
    
    coords = this->getCartesianCoordinates(state);
    vel = this->getCartesianVelocities(state);

    // NOTE: For the compound pendulum the center of mass of each rod is the
    //       midpoint between the two extremities.
    
    // Base energy so that E_tot = 0 if the pendulum is vertical and still.
    energy = this->M1 * this->g * this->L1 + this->M2 * this->g * this->L2;
    // Potential gravitational.
    energy += - this->M1 * this->g * coords[3] - this->M2 * this->g * coords[7];
    // Kinetic translational.
    energy += this->M1 * (pow(vel[2], 2) + pow(vel[3], 2)) / 2.0
           +  this->M2 * (pow(vel[6], 2) + pow(vel[7], 2)) / 2.0;
    // Kinetic rotational.
    energy += this->M1 * pow(this->L1, 2) / 12.0 * pow(state.w1, 2) / 2.0
           +  this->M2 * pow(this->L2, 2) / 12.0 * pow(state.w2, 2) / 2.0;

    return energy;
}