#include <stdio.h>
#include "output.h"

void print_mesh(struct mesh *mesh);

void save_to_dtm(struct mesh **meshes, int meshes_count, char *filename) {
    for (int i = 0; i < meshes_count; ++i) {
        print_mesh(meshes[i]);
    }
}

void print_mesh(struct mesh *mesh) {
    printf("MESH:\n");
    for (int i = 0; i < mesh->counter; ++i) {
#ifdef DEBUG
#include "triangle.h"
        verify_triangle(&mesh->triangles[i], mesh);
#endif
        printf("(%d, %d, %d), (%d, %d, %d), (%d, %d, %d)\n",
               mesh->triangles[i].vertices[0].x, mesh->triangles[i].vertices[0].y, mesh->triangles[i].vertices[0].z,
               mesh->triangles[i].vertices[1].x, mesh->triangles[i].vertices[1].y, mesh->triangles[i].vertices[1].z,
               mesh->triangles[i].vertices[2].x, mesh->triangles[i].vertices[2].y, mesh->triangles[i].vertices[2].z);
    }
    printf("\n\n\n");
}
