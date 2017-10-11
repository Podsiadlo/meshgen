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
        printf("(%d, %d, %d), (%d, %d, %d), (%d, %d, %d)\n",
               mesh->triangles[i]->a.x, mesh->triangles[i]->a.y, mesh->triangles[i]->a.z,
               mesh->triangles[i]->b.x, mesh->triangles[i]->b.y, mesh->triangles[i]->b.z,
               mesh->triangles[i]->c.x, mesh->triangles[i]->c.y, mesh->triangles[i]->c.z);
    }
    printf("\n\n\n");
}
