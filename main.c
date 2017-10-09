#include <stdlib.h>
#include "math.h"
#include "input.h"
#include "mesh.h"
#include "output.h"

struct mesh **split_map(short **map, int width, int length, int gcd);
int gcd(int a, int b);


int main(int argc, char **argv) {
    double begin_longitude = 50.5;
    double begin_latitude = 19.3;
    double end_longitude = 50.7;
    double end_latitude = 19.5;
    char *map_dir = ".";
    char *output_filename = "result.dtm";

    short **map = read_map(begin_longitude, begin_latitude, end_longitude, end_latitude, map_dir);
    int width = (int) (VALUES_IN_DEGREE * fabs(end_latitude - begin_longitude));
    int length = (int) (VALUES_IN_DEGREE * fabs(end_latitude - begin_longitude));
    int map_gcd = gcd(width, length);
    struct mesh **meshes = split_map(map, width, length, map_gcd);
    for (int i = 0; i < width * length; ++i) {
        //TODO: Do magic here
    }
    save_to_dtm(meshes, output_filename);

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
            meshes[i * rows + j] = generate_mesh(map, j * gcd, i * gcd, gcd);
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