#include "mesh.h"

#include <stdlib.h>
#include <string.h>

#include "refinement.h"
#include "triangle.h"

void
generate_first_triangles(int square_no, int size, int columns, int rows,
                         struct mesh *mesh)
{
    int square_row = square_no / columns;
    int square_col = square_no % columns;
    double first_data_row = square_row * size;
    double first_data_col = square_col * size;

    struct triangle *first = get_new_triangle(mesh);
    init_triangle(first, first_data_col, first_data_row + size - 1, first_data_col + size - 1,
                  first_data_row + size - 1, first_data_col + size - 1, first_data_row,
                  mesh->map);
    struct triangle *second = get_new_triangle(mesh);
    init_triangle(second, first_data_col + size - 1, first_data_row, first_data_col, first_data_row,
                  first_data_col, first_data_row + size - 1, mesh->map);

    first->neighbours[2] = second->index;
    first->neighbours[1] = square_col == columns - 1 ? -1 : (square_no + 1) * 2 + 1;
    first->neighbours[0] = square_row == rows - 1 ? -1 : (square_no + columns) * 2 + 1;
    second->neighbours[2] = first->index;
    second->neighbours[1] = square_col % columns == 0 ? -1 : (square_no - 1) * 2;
    second->neighbours[0] = square_row / columns == 0 ? -1 : (square_no - columns) * 2;
}

void
refine_new_mesh(struct mesh *mesh)
{
    bool finish = false;
    while (!finish) {
        bool modified = false;
        for (size_t i = 0; i < mesh->counter; ++i) {
            if (refine_if_required(&(mesh->triangles[i]), mesh)) {
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
    mesh->triangles[mesh->counter].index = mesh->counter;
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
