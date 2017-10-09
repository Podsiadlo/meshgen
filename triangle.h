//
// Created by krzysiek on 09.10.17.
//

#ifndef MESHGEN_TRIANGLE_H
#define MESHGEN_TRIANGLE_H

#include <stdbool.h>
#include "point.h"

struct triangle {//FIXME
    struct point *a;//deprecated
    struct point *b;
    struct point *c;

    struct point a2;
    struct point b2;
    struct point c2;

    struct triangle *child_ab;
    struct triangle *child_bc;
    struct triangle *child_ac;
    short longest; //0=ab, 1=bc, 2=ac
};

bool final_step(struct triangle *triangle);
void refine(struct triangle *parent);
struct triangle *create_triangle(int a_x, int a_y, int b_x, int b_y,  int c_x, int c_y, short** map);
struct triangle *split(struct triangle *triangle1, struct triangle *triangle2);
void fix_triangle(struct triangle *triangle);
void fix_longest(struct triangle *triangle);
void fix_childs(struct triangle *triangle);
struct triangle *get_next_triangle(struct triangle *triangle);
struct point *get_longest_edge_midsection(const struct triangle *triangle);

#endif //MESHGEN_TRIANGLE_H
