#include "output.h"

#include <stdio.h>

void
print_mesh(struct mesh *mesh);

void
save_to_dtm(struct mesh **meshes, int meshes_count, char *filename) {
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
    int point_counter = 0;
    int triangles_counter = 0;

    for (int i = 0; i < meshes_count; ++i) {
//        print_mesh(meshes[i]);
        get_triangles(meshes[i], &triangles, &triangles_counter, &triangles_size, &points, &point_counter,
                      &points_size);
    }

    FILE *file = fopen(filename, "w");
    fprintf(file, vtk0, point_counter);
    for (int l = 0; l < point_counter; ++l) {
        fprintf(file, "%d %d %d\n", points[l]->x, points[l]->y, points[l]->z);
    }
    fprintf(file, vtk1, triangles_counter, 3 * triangles_counter);
    for (int j = 0; j < triangles_counter; ++j) {
        fprintf(file, "%d %d %d\n", triangles[j]->points[0], triangles[j]->points[1], triangles[j]->points[2]);
    }

//    fwrite(vtk0, sizeof(char), strlen(vtk0), file);
//    fwrite(point_counter, sizeof(char), strlen(vtk0), file);

    fclose(file);
    for (int k = 0; k < triangles_counter; ++k) {
        free(triangles[k]);
    }
    free(points);
    free(triangles);
}

void get_triangles(struct mesh *mesh, struct three ***triangles, int *triangles_counter, size_t *triangles_size,
                   struct point ***points, int *point_counter, size_t *points_size) {
    for (int i = 0; i < mesh->counter; ++i) {
        struct three *triangle = (struct three *) malloc(sizeof(struct three));
        triangle->points[0] = get_point_number(&mesh->triangles[i].vertices[0], points, point_counter, points_size);
        triangle->points[1] = get_point_number(&mesh->triangles[i].vertices[1], points, point_counter, points_size);
        triangle->points[2] = get_point_number(&mesh->triangles[i].vertices[2], points, point_counter, points_size);
        if (*triangles_counter >= *triangles_size) {
            *triangles_size *= 2;
            *triangles = (struct three **) realloc(*triangles, *triangles_size);
        }

#ifdef DEBUG
        for (int j = 0; j < *triangles_counter; ++j) {
            if (triangle->points[0] == (*triangles)[j]->points[0] ||
                triangle->points[0] == (*triangles)[j]->points[1] ||
                triangle->points[0] == (*triangles)[j]->points[2]) {

                if (triangle->points[1] == (*triangles)[j]->points[0] ||
                    triangle->points[1] == (*triangles)[j]->points[1] ||
                    triangle->points[1] == (*triangles)[j]->points[2]) {

                    if (triangle->points[2] == (*triangles)[j]->points[0] ||
                        triangle->points[2] == (*triangles)[j]->points[1] ||
                        triangle->points[2] == (*triangles)[j]->points[2]) {

                        printf("Found duplicated triangle: %d %d %d\n",
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

int get_point_number(struct point *point, struct point ***points, int *points_counter, size_t *points_size) {
    for (int i = 0; i < *points_counter; ++i) {
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
print_mesh(struct mesh *mesh) {
    printf("MESH:\n");
    for (size_t i = 0; i < mesh->counter; ++i) {
#ifdef DEBUG
#include "triangle.h"
        verify_triangle(&mesh->triangles[i], mesh);
#endif
        printf(
                "(%d, %d, %d), (%d, %d, %d), (%d, %d, %d)\n",
                mesh->triangles[i].vertices[0].x, mesh->triangles[i].vertices[0].y,
                mesh->triangles[i].vertices[0].z, mesh->triangles[i].vertices[1].x,
                mesh->triangles[i].vertices[1].y, mesh->triangles[i].vertices[1].z,
                mesh->triangles[i].vertices[2].x, mesh->triangles[i].vertices[2].y,
                mesh->triangles[i].vertices[2].z);
    }
    printf("\n\n\n");
}
