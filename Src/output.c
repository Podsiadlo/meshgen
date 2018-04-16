#include "output.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "mesh.h"

void
save_to_inp(struct mesh *mesh, char *filename)
{
    char *preambule = "%d %d 0 0 0"; //<number_of_points> <number_of_triangles> 0 0 0
    char *point = "\n%d %lf %lf %lf"; //<vertex_id> <x> <y> <z>
    char *triangle = "\n%d 0 tri %d %d %d"; //<triangle_id>  0 tri <vertex_id> <vertex_id> <vertex_id>

    size_t points_size = 1000; //TODO: improve memory management
    size_t triangles_size = 1000;
    struct point **points = (struct point **) malloc(points_size * sizeof(struct point *));
    struct three **triangles = (struct three **) malloc(triangles_size * sizeof(struct three *));
    size_t point_counter = 0;
    size_t triangles_counter = 0;

    get_triangles(mesh, &triangles, &triangles_counter, &triangles_size, &points, &point_counter, &points_size);


    FILE *file;
    if ((file = fopen(filename, "w")) == NULL) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }
    fprintf(file, preambule, point_counter, triangles_counter);
    for (size_t l = 0; l < point_counter; ++l) {
        fprintf(file, point, l, points[l]->x, points[l]->y, points[l]->z);
    }
    for (size_t j = 0; j < triangles_counter; ++j) {
        fprintf(file, triangle, j, triangles[j]->points[0], triangles[j]->points[1], triangles[j]->points[2]);
    }

    if (fclose(file) != 0) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }
    for (size_t k = 0; k < triangles_counter; ++k) {
        free(triangles[k]);
    }
    free(points);
    free(triangles);

}

void
save_to_dtm(struct mesh *mesh, char *filename) //FIXME
{
    char *vtk0 = "# vtk DataFile Version 2.0\n"
            "Map\n"
            "ASCII\n"
            "DATASET POLYDATA\n"
            "POINTS %d float\n";
    char *vtk1 = "POLYGONS %d %d\n";


    size_t points_size = 1000;
    size_t triangles_size = 1000;
    struct point **points = (struct point **) malloc(points_size * sizeof(struct point *));
    struct three **triangles = (struct three **) malloc(triangles_size * sizeof(struct three *));
    size_t point_counter = 0;
    size_t triangles_counter = 0;

    get_triangles(mesh, &triangles, &triangles_counter, &triangles_size, &points, &point_counter, &points_size);


    FILE *file;
    if ((file = fopen(filename, "w")) == NULL) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }
    fprintf(file, vtk0, point_counter);
    for (size_t l = 0; l < point_counter; ++l) {
        fprintf(file, "%lf %lf %lf\n", points[l]->x, points[l]->y, points[l]->z);
    }
    fprintf(file, vtk1, triangles_counter, 3 * triangles_counter);
    for (size_t j = 0; j < triangles_counter; ++j) {
        fprintf(file, "%ld %ld %ld\n", triangles[j]->points[0], triangles[j]->points[1], triangles[j]->points[2]);
    }

    if (fclose(file) != 0) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }
    for (size_t k = 0; k < triangles_counter; ++k) {
        free(triangles[k]);
    }
    free(points);
    free(triangles);
}

void get_triangles(struct mesh *mesh, struct three ***triangles, size_t *triangles_counter, size_t *triangles_size,
                   struct point ***points, size_t *point_counter, size_t *points_size)
{
    for (size_t i = 0; i < mesh->counter; ++i) {
        struct three *triangle = (struct three *) malloc(sizeof(struct three));
        triangle->points[0] = get_point_number(&mesh->triangles[i].vertices[0], points, point_counter, points_size);
        triangle->points[1] = get_point_number(&mesh->triangles[i].vertices[1], points, point_counter, points_size);
        triangle->points[2] = get_point_number(&mesh->triangles[i].vertices[2], points, point_counter, points_size);
        if (*triangles_counter >= *triangles_size) {
            *triangles_size *= 2;
            *triangles = (struct three **) realloc(*triangles, *triangles_size);
        }

#ifndef NDEBUG
        for (size_t j = 0; j < *triangles_counter; ++j) {
            if (triangle->points[0] == (*triangles)[j]->points[0] ||
                triangle->points[0] == (*triangles)[j]->points[1] ||
                triangle->points[0] == (*triangles)[j]->points[2]) {

                if (triangle->points[1] == (*triangles)[j]->points[0] ||
                    triangle->points[1] == (*triangles)[j]->points[1] ||
                    triangle->points[1] == (*triangles)[j]->points[2]) {

                    if (triangle->points[2] == (*triangles)[j]->points[0] ||
                        triangle->points[2] == (*triangles)[j]->points[1] ||
                        triangle->points[2] == (*triangles)[j]->points[2]) {

                        printf("Found duplicated triangle: %ld %ld %ld\n",
                                triangle->points[0], triangle->points[1], triangle->points[2]);
                        free(triangle);
                        return;
                    }
                }
            }
        }
#endif

        (*triangles)[(*triangles_counter)++] = triangle;
    }
}

size_t get_point_number(struct point *point, struct point ***points, size_t *points_counter, size_t *points_size)
{
    for (size_t i = 0; i < *points_counter; ++i) {
        if (point->x == (*points)[i]->x && point->y == (*points)[i]->y) {
            return i;
        }
    }
    if (*points_counter >= *points_size) {
        *points_size *= 2;
        *points = (struct point **) realloc(*points, *points_size);
    }
    (*points)[*points_counter] = point;
    return (*points_counter)++;
}


void
print_mesh(struct mesh *mesh)
{
    printf("MESH:\n");
    for (size_t i = 0; i < mesh->counter; ++i) {
#ifndef NDEBUG
        verify_triangle(&mesh->triangles[i], mesh);
#endif
        printf(
                "(%lf, %lf, %lf), (%lf, %lf, %lf), (%lf, %lf, %lf)\n",
                mesh->triangles[i].vertices[0].x, mesh->triangles[i].vertices[0].y,
                mesh->triangles[i].vertices[0].z, mesh->triangles[i].vertices[1].x,
                mesh->triangles[i].vertices[1].y, mesh->triangles[i].vertices[1].z,
                mesh->triangles[i].vertices[2].x, mesh->triangles[i].vertices[2].y,
                mesh->triangles[i].vertices[2].z);
    }
    printf("\n\n\n");
}
