#ifndef MESHGEN_TRIANGLE_H
#define MESHGEN_TRIANGLE_H

static const int EPSILON = 5;

#include <stdbool.h>
#include "point.h"
#include "mesh.h"

struct triangle {
    int index;
    struct point vertices[3]; // P0, P1, P2
    int children[3]; //0 -> P0P1, 1 -> P1P2, 2 -> P2P0
    short longest; //0 -> P0P1, 1 -> P1P2, 2 -> P2P0
};

struct triangle *init_triangle(struct triangle *triangle, unsigned int a_x, unsigned int a_y,
                               unsigned int b_x, unsigned int b_y, unsigned int c_x, unsigned int c_y,
                               const short **map);

void fix_longest(struct triangle *triangle);

int get_height_mean(const struct triangle *triangle);

short get_height_of_center(const struct triangle *triangle, const short **map);

int get_next_triangle_index(struct triangle *triangle);

void get_longest_edge_midsection(struct point *destination, struct triangle *triangle);

#ifdef DEBUG
void verify_neighbours(struct triangle *triangle, struct mesh *mesh);
#endif

#endif //MESHGEN_TRIANGLE_H
