#include "DataPoint.hpp"

DataPoint::DataPoint(double x, double y, double val, double size) {
    update(x, y, val, size);
}

void DataPoint::update(double x, double y, double val, double size) {
    this->x = x;
    this->y = y;
    this->val = val;
    this->size = size;
}