#include <cmath>
#include <string>
#include <iostream>
#include <sstream>
#include "StateVector.hpp"
#include "DoublePendulum.hpp"

DoublePendulum::DoublePendulum(double M1, double M2, double L1, double L2, double dt, double g) {
    // System parameters.
    this->M1 = M1;
    this->M2 = M2;
    this->L1 = L1;
    this->L2 = L2;

    // Environment paramenters.
    this->dt = dt;
    this->g = g;
};

DoublePendulum::~DoublePendulum() { };

/*
 * Calculates the next state vector based on the current one and the equation of motion in the state form,
 * using a Runge Kutta method of the 4th order.
 */
StateVector DoublePendulum::calcNextState(StateVector currState) {
    StateVector Y1, Y2, Y3, Y4;
    StateVector k1, k2, k3, k4;
    StateVector nextState;
    
    Y1 = currState;
    k1 = motionEquationStateForm(Y1);
    Y2 = currState + k1 * this->dt/2.0;

    k2 = motionEquationStateForm(Y2);
    Y3 = currState + k2 * this->dt/2.0;

    k3 = motionEquationStateForm(Y3);
    Y4 = currState + k3 * this->dt;

    k4 = motionEquationStateForm(Y4);
    Y4 = currState + k3 * this->dt;
    nextState = currState + (k1 + k2 * 2 + k3 * 2 + k4) * this->dt/6.0;

    return nextState;
}

std::array<double, DoublePendulum::N_COORDS> DoublePendulum::getCartesianCoordinates(StateVector state) {
    /*
     * Order of the points in the coords array: O, G1, A, G2, B.
     *
     *     O     G1     A     G2     B
     *     O------------o------------o
     */
    std::array<double, DoublePendulum::N_COORDS> coords;
    
    // Fixed origin: O(x,y)
    coords[0] = 0;
    coords[1] = 0;
    // Extremity of the first rod and junction between the two: A(x,y)
    coords[4] = this->L1 * sin(state.a1);
    coords[5] = this->L1 * cos(state.a1);
    // Extremity of the second rod: B(x,y)
    coords[8] = coords[4] + this->L2 * sin(state.a2);
    coords[9] = coords[5] + this->L2 * cos(state.a2);
    // Midpoint of the first rod: G1(x,y)
    coords[2] = (coords[0] + coords[4]) / 2;
    coords[3] = (coords[1] + coords[5]) / 2;
    // Midpoint of the second rod: G2(x,y)
    coords[6] = (coords[4] + coords[8]) / 2;
    coords[7] = (coords[5] + coords[9]) / 2;

    return coords;
}

std::array<double, DoublePendulum::N_COORDS> DoublePendulum::getCartesianVelocities(StateVector state) {
    /*
     * Order of the points in the coords array: O, G1, A, G2, B.
     *
     *     O     G1     A     G2     B
     *     O------------o------------o
     */

    std::array<double, DoublePendulum::N_COORDS> vel;
    
    // Fixed origin: O(x,y)
    vel[0] = 0;
    vel[1] = 0;
    // Extremity of the first rod and junction between the two: A(x,y)
    vel[4] = + this->L1 * cos(state.a1) * state.w1;
    vel[5] = - this->L1 * sin(state.a1) * state.w1;
    // Extremity of the second rod: B(x,y)
    vel[8] = vel[4] + this->L2 * cos(state.a2) * state.w2;
    vel[9] = vel[5] - this->L2 * sin(state.a2) * state.w2;
    // Midpoint of the first rod: G1(x,y)
    vel[2] = (vel[0] + vel[4]) / 2;
    vel[3] = (vel[1] + vel[5]) / 2;
    // Midpoint of the second rod: G2(x,y)
    vel[6] = (vel[4] + vel[8]) / 2;
    vel[7] = (vel[5] + vel[9]) / 2;

    return vel;
}

std::string DoublePendulum::getTextOutput(StateVector state, const std::string &separator) {
    // Output order: x_O, y_O, x_A, y_A, x_B, y_B, E_tot

    std::array<double, DoublePendulum::N_COORDS> coords;
    std::ostringstream os;
    
    coords = this->getCartesianCoordinates(state);
    os << coords[0] << separator << coords[1] << separator << coords[4] << separator << coords[5] << separator << coords[8] << separator << coords[9] << separator << this->getEnergy(state) << "\n";

    return os.str();
}