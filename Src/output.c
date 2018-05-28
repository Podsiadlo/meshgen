#include "output.h"

#include <string.h>
#include <errno.h>
#include <math.h>

#include "mesh.h"

#define NDEBUG

void shift(int from, int to, size_t *array);

void
save_to_inp(struct mesh *mesh, char *filename) {
    char *preambule = "%d %d 0 0 0"; //<number_of_points> <number_of_triangles> 0 0 0
    char *point = "\n%d %lf %lf %lf"; //<vertex_id> <x> <y> <z>
    char *triangle = "\n%d 0 tri %d %d %d"; //<triangle_id>  0 tri <vertex_id> <vertex_id> <vertex_id>

    size_t points_size = mesh->counter + 2;
    size_t triangles_size = mesh->counter;
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
save_to_smesh(struct mesh *mesh, char *filename) {
    double height = 2000;

    char *point_preambule = "%d 3 0 0"; //<number_of_points> <dimensions> <#_of_attributes> <boundary_markers>
    char *facet_preambule = "\n%d 1"; //<number_of_facets> <boundary_markers>
    char *point = "\n%d %lf %lf %lf"; //<vertex_id> <x> <y> <z>
    char *triangle = "\n3 %d %d %d %d"; //<#_of_corners> <vertex_id> <vertex_id> <vertex_id> <face>
    char *surface_facet = "\n4 %d %d %d %d %d"; //<#_of_corners> <vertex_id> <vertex_id> <vertex_id>
    char *ending = "\n0\n0"; //<holes> <region_attributes>

    size_t points_size = mesh->counter + 6;
    size_t triangles_size = mesh->counter;
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
    fprintf(file, "\n"); //FIXME: I don't understand why, but without it, it throws SIGSEGV later
    fflush(file);

    //Points
    struct point surface[4];
    surface[0].x = 0.;
    surface[0].y = 0.;
    surface[0].z = height;
    surface[1].x = 0.;
    surface[1].y = mesh->map->length - 1;
    surface[1].z = height;
    surface[2].x = mesh->map->width - 1;
    surface[2].y = 0.;
    surface[2].z = height;
    surface[3].x = mesh->map->width - 1;
    surface[3].y = mesh->map->length - 1;
    surface[3].z = height;
    get_new_point_index(&surface[0], &points, &point_counter, &points_size);
    get_new_point_index(&surface[1], &points, &point_counter, &points_size);
    get_new_point_index(&surface[2], &points, &point_counter, &points_size);
    get_new_point_index(&surface[3], &points, &point_counter, &points_size);
    fprintf(file, point_preambule, point_counter);
    fflush(file);
    for (size_t l = 0; l < point_counter; ++l) {
        fprintf(file, point, l, points[l]->x, points[l]->y, points[l]->z);
        fflush(file);
    }

    //Facets
    fprintf(file, facet_preambule, triangles_counter + 5);
    fflush(file);
    for (size_t j = 0; j < triangles_counter; ++j) {
        fprintf(file, triangle, triangles[j]->points[0], triangles[j]->points[1], triangles[j]->points[2], 1);
        fflush(file);
    }
    fprintf(file, surface_facet, point_counter - 3, point_counter - 4, point_counter - 2, point_counter - 1, 6);
    fflush(file);
    write_border_facet(0., 0, 2, file, points, point_counter);
    write_border_facet(mesh->map->width - 1, 0, 3, file, points, point_counter);
    write_border_facet(0., 1, 4, file, points, point_counter);
    write_border_facet(mesh->map->length - 1, 1, 5, file, points, point_counter);

    fprintf(file, ending);

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

void
write_border_facet(double border, int coordinate, int wall_number, FILE *file, struct point **points,
                   size_t point_counter)
{
    size_t buffer[1000]; //TODO improve memory management
    int border_points = find_border_points(border, coordinate, buffer, points, point_counter);
    sort_points(border_points - 1, buffer, coordinate == 0 ? 1 : 0, points); // Maybe I don't need -1
    fprintf(file, "\n%d", border_points);
    fflush(file);
    for (int i = 0; i < border_points; ++i) {
        fprintf(file, " %d", (int) buffer[i]);
        fflush(file);
    }
    fprintf(file, " %d", wall_number);
    fflush(file);
}

void
sort_points(int size, size_t indices[], int coordinate, struct point **points)
{

    for(int i = 0; i<size; ++i) {
        for(int j = 0; j<i; ++j) {
            double j_elem = get_coordinate(coordinate, points[indices[j]]);
            double i_elem = get_coordinate(coordinate, points[indices[i]]);
            if(j_elem > i_elem - EPSILON
               || (fabs(i_elem-j_elem) < 0
                   && get_coordinate(coordinate == 0 ? 1 : 0, points[indices[j]])
                      > get_coordinate(coordinate == 0 ? 1 : 0, points[indices[j]]))) {
                size_t tmp = indices[i];
                shift(j, i, indices);
                indices[j] = tmp;
            }
        }
    }
}

void
shift(int from, int to, size_t *array)
{
    for (int i = to; i > from; --i) {
        array[i] = array[i - 1];
    }
}

void
get_triangles(struct mesh *mesh, struct three ***triangles, size_t *triangles_counter, size_t *triangles_size,
              struct point ***points, size_t *point_counter, size_t *points_size) {
    for (size_t i = 0; i < mesh->counter; ++i) {
        struct three *triangle = (struct three *) malloc(sizeof(struct three));
        triangle->points[0] = get_point_index(&mesh->triangles[i].vertices[0], points, point_counter, points_size);
        triangle->points[1] = get_point_index(&mesh->triangles[i].vertices[1], points, point_counter, points_size);
        triangle->points[2] = get_point_index(&mesh->triangles[i].vertices[2], points, point_counter, points_size);
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

int
find_border_points(double border, int coordinate, size_t *buffer, struct point **points, size_t points_counter) {
    int points_found = 0;

    for (size_t i = 0; i < points_counter; ++i) {
        if (fabs((coordinate == 0 ? points[i]->x : points[i]->y) - border) < EPSILON) {
            buffer[points_found++] = i;
        }
    }

    return points_found;
}

size_t
get_point_index(struct point *point, struct point ***points, size_t *points_counter, size_t *points_size) {
    for (size_t i = 0; i < *points_counter; ++i) {
        if (point->x == (*points)[i]->x && point->y == (*points)[i]->y) {
            return i;
        }
    }
    return get_new_point_index(point, points, points_counter, points_size);
}

size_t get_new_point_index(struct point *point, struct point ***points, size_t *points_counter, size_t *points_size) {
    if (*points_counter >= *points_size) { //TODO:
        *points_size *= 2;
        struct point **realloc_check = realloc(*points, *points_size);
        if (realloc_check == NULL) {
            fprintf(stderr, "Error during points reallocation.\n");
            exit(6);
        }
        *points = realloc_check;
    }
    (*points)[*points_counter] = point;
    return (*points_counter)++;
}


void
print_mesh(struct mesh *mesh) {
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
