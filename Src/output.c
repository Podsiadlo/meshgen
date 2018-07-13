#include "output.h"

#include <string.h>
#include <errno.h>
#include <math.h>

#include "mesh.h"
#include "utils.h"
#include "libmgrs/utm.h"

#ifndef NDEBUG
#define NDEBUG
#endif

void
save_to_inp(struct mesh *mesh, char *filename, bool utm)
{
    char *preambule = "%d %d 0 0 0"; //<number_of_points> <number_of_triangles> 0 0 0
    char *point = "\n%d %lf %lf %lf"; //<vertex_id> <x> <y> <z>
    char *triangle = "\n%d %d tri %d %d %d"; //<triangle_id>  <material> tri <vertex_id> <vertex_id> <vertex_id>

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
    print_points(utm, point, points, point_counter, file);

    for (size_t j = 0; j < triangles_counter; ++j) {
        fprintf(file, triangle, j, 0, triangles[j]->points[0], triangles[j]->points[1], triangles[j]->points[2]);
    }

    output_cleanup(points, triangles, triangles_counter, file);
}

void
save_to_smesh(struct mesh *mesh, char *filename, bool pre_utm, bool utm)
{
    double height = 3000;

    char *point_preambule = "%d 3 0 0"; //<number_of_points> <dimensions> <#_of_attributes> <boundary_markers>
    char *facet_preambule = "\n%d 1"; //<number_of_facets> <boundary_markers>
    char *point = (pre_utm || utm) ? "\n%d %.2lf %.2lf %.2lf" : "\n%d %lf %lf %.2lf"; //<vertex_id> <x> <y> <z>
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

    struct point *surface = generate_surface(mesh->map, pre_utm, height, &points, &point_counter, &points_size);

    FILE *file;
    if ((file = fopen(filename, "w")) == NULL) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }

    fprintf(file, point_preambule, point_counter);
    fflush(file);

    print_points(utm, point, points, point_counter, file);

    //Facets
    fprintf(file, facet_preambule, triangles_counter + 5);
    fflush(file);

    for (size_t j = 0; j < triangles_counter; ++j) {
        fprintf(file, triangle, triangles[j]->points[0], triangles[j]->points[1], triangles[j]->points[2], 1);
        fflush(file);
    }
    fprintf(file, surface_facet, point_counter - 3, point_counter - 4, point_counter - 2, point_counter - 1, 6);
    fflush(file);
    write_border_facet(2, 1, file, points, point_counter); //south
    write_border_facet(3, 0, file, points, point_counter); //east
    write_border_facet(4, 1, file, points, point_counter); //north
    write_border_facet(5, 0, file, points, point_counter); //west

    fprintf(file, ending);

    free(surface);
    output_cleanup(points, triangles, triangles_counter, file);
}

struct point *
generate_surface(struct map *map, bool pre_utm, double height,
                 struct point ***points, size_t *point_counter, size_t *points_size)
{
    double south_border = map->north_border - (map->length - 1) * map->cell_length;
    double north_border = map->north_border;
    double east_border = map->west_border + (map->width - 1) * map->cell_width;
    double west_border = map->west_border;
    struct point *surface = malloc(4 * sizeof(struct point));
    surface[0].x = west_border;
    surface[0].y = south_border;
    surface[0].z = height;
    surface[0].border = 36;
    surface[1].x = west_border;
    surface[1].y = north_border;
    surface[1].z = height;
    surface[1].border = 48;
    surface[2].x = east_border;
    surface[2].y = south_border;
    surface[2].z = height;
    surface[2].border = 12;
    surface[3].x = east_border;
    surface[3].y = north_border;
    surface[3].z = height;
    surface[3].border = 24;

    get_new_point_index(&surface[0], points, point_counter, points_size);
    get_new_point_index(&surface[1], points, point_counter, points_size);
    get_new_point_index(&surface[2], points, point_counter, points_size);
    get_new_point_index(&surface[3], points, point_counter, points_size);

    for (int i = 0; i < 4; ++i) {
        convert_if_required(&surface[i], pre_utm);
    }
    return surface;
}

void output_cleanup(struct point **points, struct three **triangles, size_t triangles_counter, FILE *file)
{
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
print_points(bool convert_to_utm, const char *point_template, struct point *const *points, size_t point_counter,
             FILE *file)
{
    for (size_t l = 0; l < point_counter; ++l) {
        convert_if_required(points[l], convert_to_utm);
        fprintf(file, point_template, l, points[l]->x, points[l]->y, points[l]->z);
        fflush(file);
    }
}

void
convert_if_required(struct point *point, bool convert_to_utm)
{
    if (convert_to_utm) {
        double x, y;
        char hemisphere;
        long zone;
        if (Convert_Geodetic_To_UTM(d2r(point->y),
                                    d2r(point->x),
                                    &zone, &hemisphere, &x, &y)) {

            fprintf(stderr, "Error during conversion.\n");
            exit(17);
        } else {
            x = round(100 * x) / 100.;
            y = round(100 * y) / 100.;
            point->x = x;
            point->y = y;
        }
    }
}

void
write_border_facet(char wall_number, int coordinate, FILE *file, struct point **points, size_t point_counter)
{
    size_t buffer[1024]; //TODO improve memory management
    int border_points = find_border_points(wall_number, buffer, points, point_counter);
    sort_points(border_points - 2, buffer, coordinate == 0 ? 1 : 0, points);
    fprintf(file, "\n%d", border_points);
    fflush(file);

    size_t tmp = buffer[border_points - 2];
    shift(0, border_points - 2, buffer);
    buffer[0] = tmp;

    for (int i = 0; i < border_points; ++i) {
        fprintf(file, " %d", (int) buffer[i]);
        fflush(file);
    }
    fprintf(file, " %d", wall_number);
    fflush(file);
}

void
get_triangles(struct mesh *mesh, struct three ***triangles, size_t *triangles_counter, size_t *triangles_size,
              struct point ***points, size_t *point_counter, size_t *points_size)
{
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
find_border_points(char border, size_t *buffer, struct point **points, size_t points_counter)
{
    int points_found = 0;

    for (size_t i = 0; i < points_counter; ++i) {
        char and = points[i]->border & (char)(1 << border);
        if (and > 0) {
//        if (equals((coordinate == 0 ? points[i]->x : points[i]->y), border)) {
            buffer[points_found++] = i;
        }
    }

    return points_found;
}

size_t
get_point_index(struct point *point, struct point ***points, size_t *points_counter, size_t *points_size)
{
    for (size_t i = 0; i < *points_counter; ++i) {
        if (point_equals(point, (*points)[i])) {
            return i;
        }
    }
    return get_new_point_index(point, points, points_counter, points_size);
}

size_t
get_new_point_index(struct point *point, struct point ***points, size_t *points_counter, size_t *points_size)
{
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


void
write_edges(struct mesh *mesh, const char *filename)
{
    FILE * f = fopen(filename, "w");

    fprintf(f, "%ld %ld 0 0 0\n", mesh->counter * 3, mesh->counter * 3);

    int nodeIdx = 0;
    for (int triIdx = 0; triIdx < mesh->counter; ++triIdx) {
        struct triangle triangle = mesh->triangles[triIdx];
        for (int i = 0; i < 3; ++i) {
            struct point point = triangle.vertices[i];
            fprintf(f, "%d %g %g %g\n", nodeIdx++, point.x, point.y, point.z);
        }
    }

    for (int triIdx = 0; triIdx < mesh->counter; ++triIdx) {
        struct triangle triangle = mesh->triangles[triIdx];
        int longestEdgeIdx = triangle.longest;
        for (int edgeIdx = 0; edgeIdx < 3; ++edgeIdx) {
//            char border = triangle.vertices[edgeIdx].border & triangle.vertices[(edgeIdx + 1) % 3].border;
//            fprintf(f, "%d %d line %d %d\n", edgeIdx + (triIdx * 3), border, edgeIdx + (triIdx * 3), ((edgeIdx + 1) % 3) + (triIdx * 3) );
            fprintf(f, "%d %d line %d %d\n",  edgeIdx + (triIdx*3), edgeIdx == longestEdgeIdx ? 0 : 1, edgeIdx + (triIdx*3), ((edgeIdx + 1)%3) + (triIdx*3) );
        }
    }

    fclose(f);
}
