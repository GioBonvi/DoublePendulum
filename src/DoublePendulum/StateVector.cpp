#include "StateVector.hpp"

StateVector &StateVector::operator= (const StateVector &sv) {
    for (std::size_t i = 0; i < this->size(); i++) {
        this->at(i) = sv[i];
    }
    return *this;
}

StateVector StateVector::operator+ (const StateVector &sv) {
    StateVector res;
    for (std::size_t i = 0; i < this->size(); i++) {
        res[i] = this->at(i) + sv[i];
    }
    return res;
}
StateVector StateVector::operator- (const StateVector &sv) {
    StateVector res;
    for (std::size_t i = 0; i < this->size(); i++) {
        res[i] = this->at(i) - sv[i];
    }
    return res;
}
StateVector StateVector::operator* (const StateVector &sv) {
    StateVector res;
    for (std::size_t i = 0; i < this->size(); i++) {
        res[i] = this->at(i) * sv[i];
    }
    return res;
}
StateVector StateVector::operator/ (const StateVector &sv) {
    StateVector res;
    for (std::size_t i = 0; i < this->size(); i++) {
        res[i] = this->at(i) / sv[i];
    }
    return res;
}

StateVector StateVector::operator+ (double a) {
    StateVector res;
    for (std::size_t i = 0; i < this->size(); i++) {
        res[i] = this->at(i) + a;
    }
    return res;
}
StateVector StateVector::operator- (double a) {
    StateVector res;
    for (std::size_t i = 0; i < this->size(); i++) {
        res[i] = this->at(i) - a;
    }
    return res;
}
StateVector StateVector::operator* (double a) {
    StateVector res;
    for (std::size_t i = 0; i < this->size(); i++) {
        res[i] = this->at(i) * a;
    }
    return res;
}
StateVector StateVector::operator/ (double a) {
    StateVector res;
    for (std::size_t i = 0; i < this->size(); i++) {
        res[i] = this->at(i) / a;
    }
    return res;
}