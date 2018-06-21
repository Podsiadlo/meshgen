#ifndef MESHGEN_MESH_H
#define MESHGEN_MESH_H

static const int INITIAL_MESH_SIZE = 50;

#include <stdlib.h>
#include "triangle.h"

struct mesh {
    struct triangle *triangles;
    size_t size;
    size_t counter;
    struct map *map;
};

struct mesh *
generate_mesh(struct map *map, size_t requested_size);

void
prepare_mesh(size_t requested_size, struct mesh *mesh);

void
refine_new_mesh(struct mesh *mesh, double tolerance, bool use_height);

void
generate_first_triangles(int square_no, double cell_length, double cell_width, size_t cols, size_t rows,
                         struct mesh *mesh);

struct triangle *
get_new_triangle(struct mesh *mesh);

struct triangle *
get_triangle(int index, struct triangle *triangles);

void
free_mesh(struct mesh *mesh);

#ifndef NDEBUG
void
verify_triangle(struct triangle *triangle, struct mesh *mesh);
#endif

#endif // MESHGEN_MESH_H
