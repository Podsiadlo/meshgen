#ifndef MESHGEN_UTILS_H
#define MESHGEN_UTILS_H

#ifndef EPSILON
#define EPSILON 1e-10
#endif

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

bool
is_lesser(double a, double b);

bool
is_greater(double a, double b);

bool
equals(const double a, const double b);

void
change_bytes_order(uint16_t *var_ptr);

void
swap_if_needed(double *should_be_lower, double *should_be_bigger);

size_t
gcd(size_t a, size_t b);

double
d2r(double degrees);

double
r2d(double radians);

void
shift(int from, int to, size_t *array);

#endif //MESHGEN_UTILS_H
