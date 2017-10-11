#ifndef MESHGEN_MESH_H
#define MESHGEN_MESH_H

static const int INITIAL_MESH_SIZE = 200;

#include "triangle.h"

struct mesh {
    struct triangle** triangles;
    unsigned int size;
    unsigned int usage;
    short** map;
};

void refine_new_mesh(struct mesh *mesh);
struct mesh *generate_mesh(short **map, int first_row, int first_col, int size);
void add_triangle(struct triangle *triangle, struct mesh *mesh);
void free_mesh(struct mesh *mesh);

#endif //MESHGEN_MESH_H
