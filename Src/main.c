#include <math.h>

#include "input.h"
#include "asc_reader.h"
#include "mesh.h"
#include "output.h"
#include <getopt.h>
#include <ctype.h>

int
main(int argc, char **argv)
{
    //default parameters
    double tolerance = 1;
    size_t requested_size = 10;

    //argument parsing
    int argument;
    while ((argument = getopt (argc, argv, "t:s:")) != -1)
        switch (argument)
        {
            case 't':
                tolerance = atof(optarg);
                break;
            case 's':
                requested_size = (size_t)atoi(optarg);
                break;
            case '?':
                if (optopt == 't' || optopt == 's')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf(stderr, "Unknown option `-%c'.\nUSAGE: meshgen -t <tolerance> -s <requsted_size>\n",
                            optopt);
                else
                    fprintf (stderr,
                             "Unknown option character `\\x%x'.\n",
                             optopt);
                return 1;
            default:
                abort ();
        }

    //data reading
    size_t width;
    size_t length;

    char *output_filename = "out/result_test2.inp";
    const double **map = (const double **)readASC(&width, &length, "Examples/test2.asc");

//    double begin_longitude = 50.5;
//    double begin_latitude = 19.3;
//    double end_longitude = 50.51;
//    double end_latitude = 19.31;
//    char *map_dir = "Examples";

//    const double **map = (const double **)read_map(begin_longitude, begin_latitude, end_longitude,
//                           end_latitude, map_dir);
//    size_t width = (size_t)round(
//        VALUES_IN_DEGREE * fabs(end_longitude - begin_longitude));
//    size_t length = (size_t)round(
//        VALUES_IN_DEGREE * fabs(end_latitude - begin_latitude));

    //Actual algorithm
    struct mesh* mesh = generate_mesh(map, width, length, requested_size);

    refine_new_mesh(mesh, tolerance);

    //writing results
    save_to_inp(mesh, output_filename);

    //cleaning memory
    free_map((double **)map, length);
    free_mesh(mesh);

    return 0;
}
