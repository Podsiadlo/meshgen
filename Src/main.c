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
    double tolerance = 30;
    size_t requested_size = 5000000;
    char *output_filename = "out/result16.smesh";
    char *input_filename = "Examples/test5.asc";
    bool read_from_ASC = false;
    double begin_longitude = 50.01;
    double begin_latitude = 19.70;
    double end_longitude = 50.3;
    double end_latitude = 19.99;
    char *map_dir = "Examples";

    //argument parsing
    int argument;
    while ((argument = getopt (argc, argv, "t:s:i:o:k:l:m:n:d:")) != -1)
        switch (argument)
        {
            case 't':
                tolerance = atof(optarg);
                break;
            case 's':
                requested_size = (size_t)atoi(optarg);
                break;
            case 'o':
                output_filename = optarg;
                break;
            case 'i':
                input_filename = optarg;
                read_from_ASC = true;
                break;
            case 'k':
                begin_latitude = atof(optarg);
                read_from_ASC = false;
                break;
            case 'l':
                end_latitude = atof(optarg);
                read_from_ASC = false;
                break;
            case 'm':
                begin_longitude = atof(optarg);
                read_from_ASC = false;
                break;
            case 'n':
                end_longitude = atof(optarg);
                read_from_ASC = false;
                break;
            case 'd':
                map_dir = optarg;
                read_from_ASC = false;
                break;
            case '?':
                if (optopt == 't' || optopt == 's')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n"
                                    "USAGE: meshgen -t <tolerance> -s <requsted_size> "
                                    "-i <data_file> -o <output_file> -d <data_dir>"
                                    "-k <begin_latitude> -l <end_latitude>"
                                    "-m <begin_longitude> -n <end_longitude>\n",
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
    struct map *map;
    if (read_from_ASC) {
        map = readASC(input_filename);
    } else {
        map = read_map(begin_longitude, begin_latitude, end_longitude,
                                   end_latitude, map_dir);
    }

    //Actual algorithm
    struct mesh* mesh = generate_mesh(map, requested_size);

    refine_new_mesh(mesh, tolerance);

    //writing results
    save_to_smesh(mesh, output_filename);

    //cleaning memory
    free_mesh(mesh);

    return 0;
}
