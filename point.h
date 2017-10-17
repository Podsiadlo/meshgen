#ifndef MESHGEN_POINT_H
#define MESHGEN_POINT_H
struct point {
    int x;
    int y;
    short z;
};

void init_point(struct point * point, int x, int y, short z);
bool point_equals(struct point *a, struct point *b);

#endif //MESHGEN_POINT_H
