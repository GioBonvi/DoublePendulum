#ifndef DATA_POINT
#define DATA_POINT

/*
 * Representation of a single data point representing its surroundings.
 * 
 * The surroundings are defined as a square of side equal to size, centered
 * in (x, y). The value val is referred to the exact (x, y) position.
 */
class DataPoint {
    public:
        double x, y, val, size;

        DataPoint() = default;
        DataPoint(double xVal, double yVal, double valVal, double sizeVal);

        void update(double xVal, double yVal, double valVal, double sizeVal);
};

#endif