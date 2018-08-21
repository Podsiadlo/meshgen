#ifndef MESHGEN_OUTPUT_H
#define MESHGEN_OUTPUT_H

#include "../DataStructures/mesh.h"
#include <stdlib.h>
#include <stdio.h>

struct three {
    size_t points[3];
};

void
save_to_inp(struct mesh *mesh, char *filename, bool utm);

void
save_to_smesh(struct mesh *mesh, char *filename, bool pre_utm, bool utm);

void
print_points(bool convert_to_utm, const char *point_template, struct point *const *points, size_t point_counter,
             FILE *file);

void
output_cleanup(struct point **points, struct three **triangles, size_t triangles_counter, FILE *file);

void
convert_if_required(struct point *point, bool convert_to_utm);

struct point *
generate_surface(struct map *map, bool pre_utm, double height, struct point ***points, size_t *point_counter,
                 size_t *points_size);

void
save_to_dtm(struct mesh *mesh, char *filename);

void
get_triangles(struct mesh *mesh, struct three ***triangles, size_t *triangles_counter, size_t *triangles_size,
                   struct point ***points, size_t *point_counter, size_t *points_size);

void
write_border_facet(char wall_number, int coordinate, FILE *file, struct point **points, size_t point_counter);

int
find_border_points(char border, size_t *buffer, struct point **points, size_t points_counter);

size_t
get_point_index(struct point *point, struct point ***points, size_t *points_counter, size_t *points_size);

size_t
get_new_point_index(struct point *point, struct point ***points, size_t *points_counter, size_t *points_size);

void
write_edges(struct mesh *mesh, const char *filename);

void
print_mesh(struct mesh *mesh);

#endif // MESHGEN_OUTPUT_H
