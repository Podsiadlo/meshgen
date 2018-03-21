#ifndef MESHGEN_OUTPUT_H
#define MESHGEN_OUTPUT_H

#include "mesh.h"
#include <stdlib.h>

struct three {
    int points[3];
};

void
save_to_dtm(struct mesh **meshes, int meshes_count, char *filename);

void get_triangles(struct mesh *mesh, struct three ***triangles, int *triangles_counter, size_t *triangles_size,
                   struct point ***points, int *point_counter, size_t *points_size);

int get_point_number(struct point *point, struct point ***points, int *points_counter, size_t *points_size);

#endif // MESHGEN_OUTPUT_H
