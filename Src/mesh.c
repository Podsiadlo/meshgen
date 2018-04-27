#include "mesh.h"

#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "refinement.h"
#include "triangle.h"

struct mesh *
generate_mesh(struct map *map, size_t requested_size)
{
    struct triangle *triangles =
            (struct triangle *)malloc(INITIAL_MESH_SIZE * sizeof(struct triangle));
    struct mesh *mesh = (struct mesh *)malloc(sizeof(struct mesh));
    mesh->triangles = triangles;
    mesh->size = INITIAL_MESH_SIZE;
    mesh->counter = 0;
    mesh->map = map;
    prepare_mesh(requested_size, mesh);
    return mesh;
}

void
prepare_mesh(size_t requested_size, struct mesh *mesh)
{
    double cell_width;
    double cell_length;
    size_t rows;
    size_t columns;

    columns = (mesh->map->width-1) / requested_size;
    if ((mesh->map->width-1) % requested_size != 0) {
        if ((double)((mesh->map->width-1) % requested_size) / requested_size > 0.5) {
            columns++;
        }
    }
    cell_width = (double)(mesh->map->width-1) / columns;

    rows = (mesh->map->length-1) / requested_size;
    if ((mesh->map->length-1) % requested_size != 0) {
        if ((double)((mesh->map->length-1) % requested_size) / requested_size > 0.5) {
            rows++;
        }
    }
    cell_length = (double)(mesh->map->length-1) / rows;

    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < columns; ++j) {
            generate_first_triangles(i * columns + j, cell_length, cell_width, columns, rows, mesh);
        }
    }
}

void
generate_first_triangles(int square_no, double cell_length, double cell_width, size_t cols, size_t rows,
                         struct mesh *mesh)
{
    size_t square_row = square_no / cols;
    size_t square_col = square_no % cols;
    double first_data_row = square_row * cell_width;
    double first_data_col = square_col * cell_length;

    struct triangle *first = get_new_triangle(mesh);
    init_triangle(first, first_data_col, first_data_row + cell_width, first_data_col + cell_length,
                  first_data_row + cell_width, first_data_col + cell_length, first_data_row,
                  mesh->map);
    struct triangle *second = get_new_triangle(mesh);
    init_triangle(second, first_data_col + cell_length, first_data_row, first_data_col, first_data_row,
                  first_data_col, first_data_row + cell_width, mesh->map);

    first->neighbours[2] = second->index;
    first->neighbours[1] = square_col == cols - 1 ? -1 : (square_no + 1) * 2 + 1;
    first->neighbours[0] = square_row == rows - 1 ? -1 : (int)(square_no + cols) * 2 + 1;
    second->neighbours[2] = first->index;
    second->neighbours[1] = square_col % cols == 0 ? -1 : (square_no - 1) * 2;
    second->neighbours[0] = square_row  == 0 ? -1 : (int)(square_no - cols) * 2;
}

void
refine_new_mesh(struct mesh *mesh, double tolerance)
{
    bool finish = false;
    while (!finish) {
        bool modified = false;
        for (size_t i = 0; i < mesh->counter; ++i) {
            if (refine_if_required(&(mesh->triangles[i]), tolerance, mesh)) {
                modified = true;
            }
        }
        finish = !modified;
    }
}

struct triangle *
get_new_triangle(struct mesh *mesh)
{
    if (mesh->counter >= mesh->size) {
        mesh->triangles =
            realloc(mesh->triangles, mesh->size * 2 * sizeof(struct triangle));
        mesh->size *= 2;
    }
    mesh->triangles[mesh->counter].index = (int)mesh->counter;
    return &(mesh->triangles[mesh->counter++]);
}

struct triangle *
get_triangle(int index, struct triangle *triangles)
{
    return index != -1 ? &(triangles[index]) : NULL;
}

void
free_mesh(struct mesh *mesh)
{
    free_map(mesh->map);
    free(mesh->triangles);
    free(mesh);
}

#ifndef NDEBUG
void
verify_triangle(struct triangle *triangle, struct mesh *mesh)
{
    if ((triangle->vertices[0].x == triangle->vertices[1].x &&
            triangle->vertices[0].x == triangle->vertices[2].x) ||
        (triangle->vertices[0].y == triangle->vertices[1].y &&
            triangle->vertices[0].y == triangle->vertices[2].y)) {
        exit(5);
    }
    for (int i = 0; i < 3; ++i) {
        if (triangle->neighbours[i] != -1) {
            struct triangle *neighbour =
                    get_triangle(triangle->neighbours[i], mesh->triangles);
            if ((!point_equals(&triangle->vertices[i],
                               &neighbour->vertices[0]) &&
                 !point_equals(&triangle->vertices[i],
                               &neighbour->vertices[1]) &&
                 !point_equals(&triangle->vertices[i],
                               &neighbour->vertices[2])) ||
                (!point_equals(&triangle->vertices[(i + 1) % 3],
                               &neighbour->vertices[0]) &&
                 !point_equals(&triangle->vertices[(i + 1) % 3],
                               &neighbour->vertices[1]) &&
                 !point_equals(&triangle->vertices[(i + 1) % 3],
                               &neighbour->vertices[2]))) {

                exit(4);
            }
            if (neighbour->neighbours[0] != triangle->index &&
                neighbour->neighbours[1] != triangle->index &&
                neighbour->neighbours[2] != triangle->index) {

                exit(4);
            }
        }
    }
}

#endif
