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
    //default arguments
    double tolerance = 100;
    size_t requested_size = 500;
    char *output_filename = "out/result9.smesh";
    char *input_filename = "Examples/test5.asc";

    //argument parsing
    int argument;
    while ((argument = getopt (argc, argv, "t:s:i:o:")) != -1)
        switch (argument)
        {
            case 't':
                tolerance = atof(optarg);
                break;
            case 's':
                requested_size = (size_t)atoi(optarg);
                break;
            case 'i':
                input_filename = optarg;
                break;
            case 'o':
                output_filename = optarg;
                break;
            case '?':
                if (optopt == 't' || optopt == 's')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n"
                                    "USAGE: meshgen -t <tolerance> -s <requsted_size> "
                                    "-i <data_file> -o <output_file>\n",
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
//    struct map *map = readASC(input_filename);


    double begin_longitude = 50.1;
    double begin_latitude = 19.7;
    double end_longitude = 50.2;
    double end_latitude = 19.9;
    char *map_dir = "Examples";

    struct map *map = read_map(begin_longitude, begin_latitude, end_longitude,
                               end_latitude, map_dir);
    //Actual algorithm
    struct mesh* mesh = generate_mesh(map, requested_size);

    refine_new_mesh(mesh, tolerance);

    //writing results
    save_to_smesh(mesh, output_filename);

    //cleaning memory
    free_mesh(mesh);

    return 0;
}
