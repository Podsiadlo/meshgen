#include <math.h>

#include "input.h"
#include "asc_reader.h"
#include "mesh.h"
#include "output.h"

int
main(int argc, char **argv)
{
//    double begin_longitude = 50.5;
//    double begin_latitude = 19.3;
//    double end_longitude = 50.51;
//    double end_latitude = 19.31;
//    char *map_dir = "Examples";
    char *output_filename = "out/result_test1.inp";
    size_t width;
    size_t length;

    const double **map = (const double **)readASC(&width, &length, "Examples/test1.asc");

//    const double **map = (const double **)read_map(begin_longitude, begin_latitude, end_longitude,
//                           end_latitude, map_dir);
//    size_t width = (size_t)round(
//        VALUES_IN_DEGREE * fabs(end_longitude - begin_longitude));
//    size_t length = (size_t)round(
//        VALUES_IN_DEGREE * fabs(end_latitude - begin_latitude));

    struct mesh* mesh = generate_mesh(map, width, length);

    refine_new_mesh(mesh);

    save_to_inp(mesh, output_filename);

    free_map((double **)map, length);
    free_mesh(mesh);

    return 0;
}
