#include "utils.h"

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
