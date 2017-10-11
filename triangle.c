#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "triangle.h"


struct triangle *create_triangle(int a_x, int a_y, int b_x, int b_y, int c_x, int c_y, struct mesh *mesh) {
    struct triangle *triangle = (struct triangle *) malloc(sizeof(struct triangle));
    init_point(&(triangle->a2), a_x, a_y, mesh->map[a_y][a_x]);
    init_point(&(triangle->b2), b_x, b_y, mesh->map[b_y][b_x]);
    init_point(&(triangle->c2), c_x, c_y, mesh->map[c_y][c_x]);
    fix_longest(triangle);
    add_triangle(triangle, mesh);
}

void fix_longest(struct triangle *triangle) {
    double ab = sqrt(pow(triangle->a2.x - triangle->b2.x, 2) + pow(triangle->a2.y - triangle->b2.y, 2));
    double bc = sqrt(pow(triangle->b2.x - triangle->c2.x, 2) + pow(triangle->b2.y - triangle->c2.y, 2));
    double ac = sqrt(pow(triangle->a2.x - triangle->c2.x, 2) + pow(triangle->a2.y - triangle->c2.y, 2));

    short longest;
    if (ab > bc) {
        if (ab > ac) {
            longest = 0;
        } else {
            longest = 2;
        }
    } else {
        if (bc > ac) {
            longest = 1;
        } else {
            longest = 2;
        }
    }
    triangle->longest = longest;
}

int get_height_mean(const struct triangle *triangle) {
    return triangle->a2.z + triangle->b2.z + triangle->c2.z / 3;
}

int get_height_of_center(const struct triangle *triangle, const short **map) {
    int x = (triangle->a2.x + triangle->b2.x + triangle->c2.x) / 3;
    int y = (triangle->a2.y + triangle->b2.y + triangle->c2.y) / 3;
    return map[y][x];
}


struct triangle *get_next_triangle(struct triangle *triangle) {
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
        a = &(triangle->a2);
        b = &(triangle->b2);
    } else if (triangle->longest == 1) {
        a = &(triangle->c2);
        b = &(triangle->b2);
    } else {
        a = &(triangle->a2);
        b = &(triangle->c2);
    }
    destination->x = (a->x + b->x) / 2;
    destination->y = (a->y + b->y) / 2;
}
