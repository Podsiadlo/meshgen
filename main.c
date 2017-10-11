#include <stdlib.h>
#include <math.h>
#include "input.h"
#include "mesh.h"
#include "output.h"

struct mesh **split_map(short **map, int width, int length, int gcd);
int gcd(int a, int b);


int main(int argc, char **argv) {
    double begin_longitude = 50.5;
    double begin_latitude = 19.3;
    double end_longitude = 50.51;
    double end_latitude = 19.31;
    char *map_dir = ".";
    char *output_filename = "result.dtm";

    short **map = read_map(begin_longitude, begin_latitude, end_longitude, end_latitude, map_dir);
    int width = (int) round(VALUES_IN_DEGREE * fabs(end_longitude - begin_longitude));
    int length = (int) round(VALUES_IN_DEGREE * fabs(end_latitude - begin_latitude));
    int map_gcd = gcd(width, length);
    struct mesh **meshes = split_map(map, width, length, map_gcd);
    int meshes_number = width * length / (map_gcd * map_gcd);
    for (int i = 0; i < meshes_number; ++i) {
        refine_new_mesh(meshes[i]);
    }
    save_to_dtm(meshes, meshes_number, output_filename);

    free_map(map, length);
    for (int j = 0; j < meshes_number; ++j) {
        free_mesh(meshes[j]);
    }
    free(meshes);

    return 0;
}

/**
 * If gcd will be relatively small, especially if it will equal 1, the result will be rather not good enough.
 */
struct mesh **split_map(short **map, int width, int length, int gcd) {
    int rows = length / gcd;
    int columns = width / gcd;
    struct mesh ** meshes = (struct mesh**)malloc(rows * columns * sizeof(struct mesh *));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            meshes[i * columns + j] = generate_mesh(map, j * gcd, i * gcd, gcd);
        }
    }
    return meshes;
}

int gcd(int a, int b) {
    do {
        if (b > a) {
            int tmp = a;
            a = b;
            b = tmp;
        }
        a -= b;
    } while (a != 0);
    return b;
}