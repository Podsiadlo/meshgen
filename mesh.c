#include <stdlib.h>
#include "mesh.h"
#include "refinement.h"

struct mesh *generate_mesh(short **map, int first_row, int first_col, int size) {
    struct mesh *mesh = (struct mesh *) malloc(sizeof(struct mesh));
    mesh->map = map;
    mesh->size = INITIAL_MESH_SIZE;
    mesh->triangles = (struct triangle **) malloc(INITIAL_MESH_SIZE * sizeof(struct triangle *));

    struct triangle *first = create_triangle(first_col, first_row + size - 1,
                                             first_col + size - 1, first_row + size - 1,
                                             first_col + size - 1, first_row, mesh);
    struct triangle *second = create_triangle(first_col + size - 1, first_row,
                                              first_col, first_row,
                                              first_col, first_row + size - 1, mesh);
    first->child_ac = second;
    first->child_bc = NULL;
    first->child_ab = NULL;
    second->child_ac = first;
    second->child_bc = NULL;
    second->child_ab = NULL;

    mesh->triangles[0] = first;
    mesh->triangles[1] = second;
    mesh->usage = 2;
    return mesh;
}

void refine_new_mesh(struct mesh* mesh) {
    bool finish = false;
    while (!finish) {
        bool modified = false;
        for (int i = 0; i < mesh->usage; ++i) {
            if (refine_if_required(mesh->triangles[i], mesh)) {
                modified = true;
            }
        }
        finish = !modified;
    }
}

void add_triangle(struct triangle *triangle, struct mesh *mesh) {
    if (mesh->usage >= mesh->size) {
        mesh->triangles = realloc(mesh->triangles, mesh->size * 2);
        mesh->size *= 2;
    }
    mesh->triangles[mesh->usage++] = triangle;
}

void free_mesh(struct mesh *mesh) {
    for (int i = 0; i < mesh->usage; ++i) {
        free(mesh->triangles[i]);
    }
    free(mesh);
}
