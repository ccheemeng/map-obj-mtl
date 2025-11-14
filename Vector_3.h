#ifndef VECTOR_3_H
#define VECTOR_3_H

struct Vector_3 {
    double x;
    double y;
    double z;

    Vector_3(double x, double y, double z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
};

#endif
