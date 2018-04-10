#ifndef MESHGEN_OUTPUT_H
#define MESHGEN_OUTPUT_H

#include "mesh.h"
#include <stdlib.h>

struct three {
    size_t points[3];
};

void
save_to_dtm(struct mesh **meshes, int meshes_count, char *filename);

void get_triangles(struct mesh *mesh, struct three ***triangles, size_t *triangles_counter, size_t *triangles_size,
                   struct point ***points, size_t *point_counter, size_t *points_size);

size_t get_point_number(struct point *point, struct point ***points, size_t *points_counter, size_t *points_size);

#endif // MESHGEN_OUTPUT_H
