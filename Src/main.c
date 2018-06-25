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
    double tolerance = 1;
    size_t requested_size = 999999;
    char *output_filename = "out/result_big3";
    char *input_filename = "Examples/test1.asc";
    bool read_from_ASC = false;
    double west_border = 19.7;
    double north_border = 50.3;
    double east_border = 19.99;
    double south_border = 50.01;
    char *map_dir = "Data";
    bool use_inp = false;
    bool utm = true;
    bool use_height = false;

    //argument parsing
    int argument;
    while ((argument = getopt (argc, argv, "t:z:i:o:n:s:w:e:d:pgh")) != -1)
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
                north_border = atof(optarg);
                read_from_ASC = false;
                break;
            case 's':
                south_border = atof(optarg);
                read_from_ASC = false;
                break;
            case 'w':
                west_border = atof(optarg);
                read_from_ASC = false;
                break;
            case 'e':
                east_border = atof(optarg);
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
            case 'h':
                use_height = true;
                break;
            case '?':
                if (optopt == 't' || optopt == 's')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n"
                                    "USAGE: meshgen -t <tolerance> -z <requsted_size> "
                                    "-i <data_file> -o <output_file> -d <data_dir>"
                                    "-n <north_border> -s <south_border>"
                                    "-w <west_border> -e <east_border> "
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
        map = read_map(west_border, north_border, east_border, south_border, map_dir);
    }

    //Actual algorithm
    struct mesh* mesh = generate_mesh(map, requested_size);

    refine_new_mesh(mesh, tolerance, use_height);

    //writing results
    char buffer[256];
    strcpy(buffer, output_filename);
    if (use_inp) {
        save_to_inp(mesh, strcat(buffer, ".inp"), utm);
    } else {
        save_to_smesh(mesh, strcat(buffer, ".smesh"), utm);
    }

    //cleaning memory
    free_mesh(mesh);

    return 0;
}
