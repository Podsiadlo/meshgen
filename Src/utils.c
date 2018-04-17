#include "utils.h"

unsigned int
gcd(unsigned int a, unsigned int b)
{
    do {
        if (b > a) {
            unsigned int tmp = a;
            a = b;
            b = tmp;
        }
        a -= b;
    } while (a != 0);
    return b;
}
