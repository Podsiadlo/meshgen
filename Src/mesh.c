#include "mesh.h"

#include <stdlib.h>
#include <string.h>

#include "refinement.h"

struct mesh *
generate_mesh(const short **map, unsigned int first_row, unsigned int first_col,
              unsigned int size)
{
    struct triangle *t =
        (struct triangle *)malloc(INITIAL_MESH_SIZE * sizeof(struct triangle));
    struct mesh m = {
        .triangles = t, .size = INITIAL_MESH_SIZE, .counter = 0, .map = map};

    struct mesh *mesh = (struct mesh *)malloc(sizeof(struct mesh));

    memcpy(mesh, &m, sizeof *mesh);

    struct triangle *first = get_new_triangle(mesh);
    init_triangle(first, first_col, first_row + size - 1, first_col + size - 1,
                  first_row + size - 1, first_col + size - 1, first_row,
                  mesh->map);
    struct triangle *second = get_new_triangle(mesh);
    init_triangle(second, first_col + size - 1, first_row, first_col, first_row,
                  first_col, first_row + size - 1, mesh->map);
    first->children[2] = second->index;
    first->children[1] = -1;
    first->children[0] = -1;
    second->children[2] = first->index;
    second->children[1] = -1;
    second->children[0] = -1;

    return mesh;
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
