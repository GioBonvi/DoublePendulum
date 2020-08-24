#ifndef STATE_VECTOR
#define STATE_VECTOR

#include <array>

/*
 * Vector representing the state of the physical system.
 * 
 * It's a simple std::array with additional features:
 *  - named access to the four state variables for ease of understanding;
 *  - operator overloads to implement operations between StateVectors in a
 *    member by member fashion;
 *  - operator overloads to implement operations between a StateVector and an
 *    arithmetic type variable (a scalar);
 */
class StateVector : public std::array<double, 4> {
    public:
        // Named access tp state variables.
        double &a1 = this->at(0);
        double &w1 = this->at(1);
        double &a2 = this->at(2);
        double &w2 = this->at(3);
        
        // Operations between StateVectors (member by member).
        StateVector& operator= (const StateVector &sv);
        StateVector operator+ (const StateVector &sv);
        StateVector operator- (const StateVector &sv);
        StateVector operator* (const StateVector &sv);
        StateVector operator/ (const StateVector &sv);
        // Operations with a scalar.
        StateVector operator+ (double a);
        StateVector operator- (double a);
        StateVector operator* (double a);
        StateVector operator/ (double a);
};

#endif