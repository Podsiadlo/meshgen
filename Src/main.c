#include "input.h"
#include "asc_reader.h"
#include "mesh.h"
#include "output.h"
#include <getopt.h>
#include <ctype.h>
#include "string.h"

int
main(int argc, char **argv)
{
    //default arguments
    double tolerance = 100;
    size_t requested_size = 500;
    char *output_filename = "out/result18";
    char *input_filename = "Examples/test5.asc";
    bool read_from_ASC = false;
    double begin_longitude = 50.01;
    double begin_latitude = 19.70;
    double end_longitude = 50.3;
    double end_latitude = 19.99;
    char *map_dir = "Examples";
    bool use_inp = false;
    bool utm = true;

    //argument parsing
    int argument;
    while ((argument = getopt (argc, argv, "t:z:i:o:n:s:w:e:d:pg")) != -1)
        switch (argument)
        {
            case 't':
                tolerance = atof(optarg);
                break;
            case 'z':
                requested_size = (size_t)atoi(optarg);
                break;
            case 'o':
                output_filename = optarg;
                break;
            case 'i':
                input_filename = optarg;
                read_from_ASC = true;
                break;
            case 'n':
                begin_latitude = atof(optarg);
                read_from_ASC = false;
                break;
            case 's':
                end_latitude = atof(optarg);
                read_from_ASC = false;
                break;
            case 'w':
                begin_longitude = atof(optarg);
                read_from_ASC = false;
                break;
            case 'e':
                end_longitude = atof(optarg);
                read_from_ASC = false;
                break;
            case 'd':
                map_dir = optarg;
                read_from_ASC = false;
                break;
            case 'p':
                use_inp = true;
                break;
            case 'g':
                utm = false;
                break;
            case '?':
                if (optopt == 't' || optopt == 's')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n"
                                    "USAGE: meshgen -t <tolerance> -z <requsted_size> "
                                    "-i <data_file> -o <output_file> -d <data_dir>"
                                    "-n <begin_latitude> -s <end_latitude>"
                                    "-w <begin_longitude> -e <end_longitude> "
                                    "-p (to use inp instead of smesh)"
                                    "-g (to output in geodetic coordinates instead of UTM)\n",
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
    char buffer[256];
    strcpy(buffer, output_filename);
    if (use_inp) {
        save_to_inp(mesh, strcat(buffer, ".inp"));
    } else {
        save_to_smesh(mesh, strcat(buffer, ".smesh"), utm);
    }

    //cleaning memory
    free_mesh(mesh);

    return 0;
}
