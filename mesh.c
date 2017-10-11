#include <stdlib.h>
#include "mesh.h"
#include "refinement.h"

struct mesh *generate_mesh(short **map, int first_row, int first_col, int size) {
    struct mesh *mesh = (struct mesh *) malloc(sizeof(struct mesh));
    mesh->map = map;
    mesh->size = INITIAL_MESH_SIZE;
    mesh->counter = 0;
    mesh->triangles = (struct triangle **) malloc(INITIAL_MESH_SIZE * sizeof(struct triangle *));

    struct triangle *first = create_triangle(first_col, first_row + size - 1,
                                             first_col + size - 1, first_row + size - 1,
                                             first_col + size - 1, first_row, map);
    struct triangle *second = create_triangle(first_col + size - 1, first_row,
                                              first_col, first_row,
                                              first_col, first_row + size - 1, map);
    first->child_ac = second;
    first->child_bc = NULL;
    first->child_ab = NULL;
    second->child_ac = first;
    second->child_bc = NULL;
    second->child_ab = NULL;

    first->child_ac = 1;
    first->child_bc = -1;
    first->child_ab = -1;
    first->index = 0;
    second->child_ac = 0;
    second->child_bc = -1;
    second->child_ab = -1;
    second->index = 1;

    add_triangle(first, mesh);
    add_triangle(second, mesh);

    return mesh;
}

void refine_new_mesh(struct mesh* mesh) {
    bool finish = false;
    while (!finish) {
        bool modified = false;
        for (int i = 0; i < mesh->counter; ++i) {
            if (refine_if_required(mesh->triangles[i], mesh)) {
                modified = true;
            }
        }
        finish = !modified;
    }
}

int add_triangle(struct triangle *triangle, struct mesh *mesh) {
    if (mesh->counter >= mesh->size) {
        mesh->triangles = realloc(mesh->triangles, mesh->size * 2);
        mesh->size *= 2;
    }
    mesh->triangles[mesh->counter] = triangle;
    return triangle->index = mesh->counter++;
}

struct triangle *get_triangle(int index, struct mesh *mesh) {
    return index != -1 ? mesh->triangles[index] : NULL;
}

void free_mesh(struct mesh *mesh) {
    for (int i = 0; i < mesh->counter; ++i) {
        free(mesh->triangles[i]);
    }
    free(mesh);
}
