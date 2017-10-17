
#include <math.h>
#include "triangle.h"


struct triangle *init_triangle(struct triangle * triangle, int a_x, int a_y, int b_x, int b_y, int c_x, int c_y,
                               const short **map) {
    init_point(&(triangle->a), a_x, a_y, map[a_y][a_x]);
    init_point(&(triangle->b), b_x, b_y, map[b_y][b_x]);
    init_point(&(triangle->c), c_x, c_y, map[c_y][c_x]);
    fix_longest(triangle);
}

void fix_longest(struct triangle *triangle) {
    double ab = sqrt(pow(triangle->a.x - triangle->b.x, 2) + pow(triangle->a.y - triangle->b.y, 2));
    double bc = sqrt(pow(triangle->b.x - triangle->c.x, 2) + pow(triangle->b.y - triangle->c.y, 2));
    double ac = sqrt(pow(triangle->a.x - triangle->c.x, 2) + pow(triangle->a.y - triangle->c.y, 2));

    short longest;
    if (ab > bc) {
        if (ab > ac) {
            longest = 0;
        } else if (fabs(ab - ac) < 0.000001) {
            if (triangle->b.y > triangle->c.y) {
                longest = 2;
            } else if (triangle->b.y < triangle->c.y) {
                longest = 0;
            } else {
                if (triangle->b.x > triangle->c.x) {
                    longest = 2;
                } else {
                    longest = 0;
                }
            }
        } else {
            longest = 2;
        }
    } else if (fabs(ab - bc) < 0.000001) {
        if (ab >= ac) {
            if (triangle->a.y > triangle->c.y) {
                longest = 1;
            } else if (triangle->a.y < triangle->c.y) {
                longest = 0;
            } else {
                if (triangle->a.x > triangle->c.x) {
                    longest = 1;
                } else {
                    longest = 0;
                }
            }
        } else {
            longest = 2;
        }
    } else {
        if (bc > ac) {
            longest = 1;
        } else if (fabs(ac - bc) < 0.000001) {
            if (triangle->b.y > triangle->a.y) {
                longest = 2;
            } else if (triangle->b.y < triangle->a.y) {
                longest = 1;
            } else {
                if (triangle->b.x > triangle->a.x) {
                    longest = 2;
                } else {
                    longest = 1;
                }
            }
        } else {
            longest = 2;
        }
    }
    triangle->longest = longest;
}

int get_height_mean(const struct triangle *triangle) {
    return (triangle->a.z + triangle->b.z + triangle->c.z) / 3;
}

int get_height_of_center(const struct triangle *triangle, const short **map) {
    int x = (triangle->a.x + triangle->b.x + triangle->c.x) / 3;
    int y = (triangle->a.y + triangle->b.y + triangle->c.y) / 3;
    return map[y][x];
}

int get_next_triangle_index(struct triangle *triangle) {
    if (triangle->longest == 0) {
        return triangle->child_ab;
    } else if (triangle->longest == 1) {
        return triangle->child_bc;
    } else {
        return triangle->child_ac;
    }
}

void get_longest_edge_midsection(struct point *destination, struct triangle *triangle) {
    struct point *a, *b;
    if (triangle->longest == 0) {
        a = &(triangle->a);
        b = &(triangle->b);
    } else if (triangle->longest == 1) {
        a = &(triangle->c);
        b = &(triangle->b);
    } else {
        a = &(triangle->a);
        b = &(triangle->c);
    }
    destination->x = (a->x + b->x) / 2;
    destination->y = (a->y + b->y) / 2;
}
