#ifndef VECTOR_3_H
#define VECTOR_3_H

template <typename T> struct Vector_3 {
    T x;
    T y;
    T z;

    Vector_3(T x, T y, T z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
};

#endif
