#ifndef MESHGEN_OUTPUT_H
#define MESHGEN_OUTPUT_H

#include "mesh.h"
#include <stdlib.h>
#include <stdio.h>

struct three {
    size_t points[3];
};

void
save_to_inp(struct mesh *mesh, char *filename);

void
save_to_smesh(struct mesh *mesh, char *filename);

void
save_to_dtm(struct mesh *mesh, char *filename);

void
get_triangles(struct mesh *mesh, struct three ***triangles, size_t *triangles_counter, size_t *triangles_size,
                   struct point ***points, size_t *point_counter, size_t *points_size);

void
write_border_facet(double border, int coordinate, FILE *file, struct point **points, size_t point_counter);

int
find_border_points(double border, int coordinate, size_t *buffer, struct point **points, size_t points_counter);

size_t
get_point_index(struct point *point, struct point ***points, size_t *points_counter, size_t *points_size);

size_t
get_new_point_index(struct point *point, struct point ***points, size_t *points_counter, size_t *points_size);

void
sort_points(int size, size_t indices[], int coordinate, struct point **points);

void
print_mesh(struct mesh *mesh);

#endif // MESHGEN_OUTPUT_H
