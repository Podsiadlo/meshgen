#include <stdlib.h>
#include <stdbool.h>

struct point {
    int x;
    int y;
};

struct point *get_point(int x, int y) {
    return (struct point *) malloc(sizeof(struct point));
}

bool point_equals(struct point *a, struct point *b) {
    return a->x == b->x && a->y == b->y;
}
