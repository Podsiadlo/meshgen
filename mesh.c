//
// Created by krzysiek on 05.10.17.
//

#include "mesh.h"

/**
 * 
 * @param map 
 * @param size 
 * @return 
 */
struct mesh *generate_mesh(short **map, int first_row, int first_col, int size) {
    struct mesh *mesh = (struct mesh *) malloc(sizeof(struct mesh));
    mesh->triangles = (struct triangle **) malloc(INITIAL_MESH_SIZE * sizeof(struct triangle *));
    mesh->size = INITIAL_MESH_SIZE;
    struct triangle *first = create_triangle(0, size - 1,
                                             size - 1, size - 1,
                                             size - 1, 0, map);
    struct triangle *second = create_triangle(size - 1, 0,
                                              0, 0,
                                              0, size - 1, map);
    mesh->triangles[0] = first;
    mesh->triangles[1] = second;
    mesh->usage = 2;
    return mesh;
}

void add_triangle(struct triangle *triangle, struct mesh *mesh) {
    if (mesh->usage >= mesh->size) {
        mesh->triangles = realloc(mesh->triangles, mesh->size * 2);
        mesh->size *= 2;
    }
    mesh->triangles[mesh->usage++] = triangle;
}

void remove_mesh(struct mesh *mesh) {
    for (int i = 0; i < mesh->size; ++i) {
        free(mesh->triangles[i]);
    }
    free(mesh);
}
