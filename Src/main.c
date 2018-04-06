#include <math.h>
#include <stdlib.h>

#include "input.h"
#include "asc_reader.h"
#include "mesh.h"
#include "output.h"

struct mesh **
split_map(const short **map, int width, int length, int gcd);

unsigned int
gcd(unsigned int a, unsigned int b);

int
main(int argc, char **argv)
{
    double begin_longitude = 50.5;
    double begin_latitude = 19.3;
    double end_longitude = 50.51;
    double end_latitude = 19.31;
    char *map_dir = "Examples";
    char *output_filename = "result.vtk";

    short **map = read_map(begin_longitude, begin_latitude, end_longitude,
                           end_latitude, map_dir);
    unsigned int width = (unsigned int)round(
        VALUES_IN_DEGREE * fabs(end_longitude - begin_longitude));
    unsigned int length = (unsigned int)round(
        VALUES_IN_DEGREE * fabs(end_latitude - begin_latitude));
//    short **map = readASC("Test/equilateral_4_4x5.inp");
//    short **map = readASC("Test/isosceles_4_6x4.inp");
//    int width = 35;
//    int length = 48;
    int map_gcd = gcd(width, length);
    struct mesh **meshes =
        split_map((const short **)map, width, length, map_gcd);
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
 * If gcd will be relatively small, especially if it will equal 1, the result
 * will be rather not good enough.
 */
struct mesh **
split_map(const short **map, int width, int length, int gcd)
{
    int rows = length / gcd;
    int columns = width / gcd;
    struct mesh **meshes =
        (struct mesh **)malloc(rows * columns * sizeof(struct mesh *));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            meshes[i * columns + j] = generate_mesh(map, j * gcd, i * gcd, gcd);
        }
    }
    return meshes;
}

unsigned int
gcd(unsigned int a, unsigned int b)
{
    do {
        if (b > a) {
            unsigned int tmp = a;
            a = b;
            b = tmp;
        }
        a -= b;
    } while (a != 0);
    return b;
}
