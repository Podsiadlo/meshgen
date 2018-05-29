#include "utils.h"

#ifndef PI
#define PI 3.14159265358979323846
#endif

size_t
gcd(size_t a, size_t b)
{
    do {
        if (b > a) {
            size_t tmp = a;
            a = b;
            b = tmp;
        }
        a -= b;
    } while (a != 0);
    return b;
}

double
d2r(double degrees)
{
    return degrees * PI / 180;
}

double
r2d(double radians)
{
    return radians * 180 / PI;
}
