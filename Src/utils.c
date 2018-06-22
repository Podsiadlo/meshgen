#include "utils.h"

#include <stdbool.h>
#include <math.h>

#ifndef PI
#define PI 3.14159265358979323846
#endif


bool
is_lesser(double a, double b)
{
    return a-b < -2*EPSILON;
}

bool
is_greater(double a, double b)
{
    return a-b > 2*EPSILON;
}

bool
equals(const double a, const double b)
{
    return fabs(a - b) < EPSILON;
}


void
change_bytes_order(uint16_t *var_ptr)
{
    uint16_t tmp = *var_ptr;
    tmp <<= 8;
    (*var_ptr) >>= 8;
    (*var_ptr) |= tmp;
}


void
swap_if_needed(double *should_be_lower, double *should_be_bigger)
{
    if ((*should_be_lower) > (*should_be_bigger)) {
        double tmp = (*should_be_lower);
        (*should_be_lower) = (*should_be_bigger);
        (*should_be_bigger) = tmp;
    }
}

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


void
shift(int from, int to, size_t *array)
{
    for (int i = to; i > from; --i) {
        array[i] = array[i - 1];
    }
}

