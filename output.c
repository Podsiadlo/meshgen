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
    for (int i = 0; i < mesh->usage; ++i) {
        printf("(%d, %d, %d), (%d, %d, %d), (%d, %d, %d)\n",
               mesh->triangles[i]->a2.x, mesh->triangles[i]->a2.y, mesh->triangles[i]->a2.z,
               mesh->triangles[i]->b2.x, mesh->triangles[i]->b2.y, mesh->triangles[i]->b2.z,
               mesh->triangles[i]->c2.x, mesh->triangles[i]->c2.y, mesh->triangles[i]->c2.z);
    }
    printf("\n\n\n");
}
