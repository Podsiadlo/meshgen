//
// Created by krzysiek on 05.10.17.
//

#ifndef MESHGEN_MESH_H
#define MESHGEN_MESH_H

static const int INITIAL_MESH_SIZE = 2;

#include "triangle.c"

struct mesh {
    struct triangle** triangles;
    size_t size;
    size_t usage;
};

struct mesh *generate_mesh(short **map, int size);
void add_triangle(struct triangle *triangle, struct mesh *mesh);
void remove_mesh(struct mesh *mesh);

#endif //MESHGEN_MESH_H
