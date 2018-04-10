#include <math.h>
#include <stdlib.h>

#include "input.h"
#include "asc_reader.h"
#include "mesh.h"
#include "output.h"

void
prepare_mesh(int width, int length, int gcd, struct mesh *mesh);

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

    const short **map = (const short **)read_map(begin_longitude, begin_latitude, end_longitude,
                           end_latitude, map_dir);
    unsigned int width = (unsigned int)round(
        VALUES_IN_DEGREE * fabs(end_longitude - begin_longitude));
    unsigned int length = (unsigned int)round(
        VALUES_IN_DEGREE * fabs(end_latitude - begin_latitude));
    int map_gcd = gcd(width, length);

    struct triangle *t =
            (struct triangle *)malloc(INITIAL_MESH_SIZE * sizeof(struct triangle));
    struct mesh mesh = {
            .triangles = t, .size = INITIAL_MESH_SIZE, .counter = 0, .map = map};
    prepare_mesh(width, length, map_gcd, &mesh);

    refine_new_mesh(&mesh);

    save_to_dtm(&mesh, output_filename);

    free_map((short**)map, length);
    free(mesh.triangles);

    return 0;
}

/**
 * If gcd will be relatively small, especially if it will equal 1, the result
 * will be rather not good enough.
 */
void
prepare_mesh(int width, int length, int gcd, struct mesh *mesh)
{
    int rows = length / gcd;
    int columns = width / gcd;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            generate_first_triangles(i * columns + j, gcd, columns, rows, mesh);
        }
    }
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
