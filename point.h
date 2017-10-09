//
// Created by krzysiek on 09.10.17.
//

#ifndef MESHGEN_POINT_H
#define MESHGEN_POINT_H
struct point {
    int x;
    int y;
    int z;
};

struct point *create_point(int x, int y);
void init_point(struct point * point, int x, int y, int z);
bool point_equals(struct point *a, struct point *b);

#endif //MESHGEN_POINT_H
