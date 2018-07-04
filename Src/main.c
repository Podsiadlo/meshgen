#include "input.h"
#include "asc_reader.h"
#include "mesh.h"
#include "output.h"
#include <getopt.h>
#include <ctype.h>
#include "string.h"
#include "utils.h"

void
set_default_config(struct config *config);

void
parse_arguments(int argc, char **argv, struct config * config);


int
main(int argc, char **argv)
{
    //argument parsing
    struct config *config = malloc(sizeof(struct config));
    set_default_config(config);
    parse_arguments(argc, argv, config);

    //data reading
    struct map *map;
    if (config->read_from_ASC) {
        map = readASC(config->input_filename);
    } else {
        map = read_map(config->west_border, config->north_border, config->east_border, config->south_border, config->map_dir);
    }
//    print_map(map);

    //Actual algorithm
    struct mesh* mesh = generate_mesh(map, config->requested_size, config->use_height);
    if (config->pre_utm) {
        convert_mesh_to_UTM(mesh);
    }
    refine_new_mesh(mesh, config->tolerance, config->use_height);

    //writing results
    char buffer[256];
    strcpy(buffer, config->output_filename);
    if (config->use_inp) {
        save_to_inp(mesh, strcat(buffer, ".inp"), config->post_utm);
    } else {
        save_to_smesh(mesh, strcat(buffer, ".smesh"), config->pre_utm, config->post_utm);
    }

    //cleaning memory
    free_mesh(mesh);
    free(config);

    return 0;
}

void set_default_config(struct config *config) {
    config->tolerance = 75;
    config->requested_size = 1000;
    config->output_filename = "out/d2";
    config->input_filename = "Examples/test1.asc";
    config->read_from_ASC = false;
    config->west_border = 20.26;
    config->north_border = 49.52;
    config->east_border = 20.44;
    config->south_border = 49.44;
    config->map_dir = "Data";
    config->use_inp = false;
    config->post_utm = true;
    config->use_height = false;
    config->pre_utm = false;
}

void
parse_arguments(int argc, char **argv, struct config * config)
{
    int argument;
    while ((argument = getopt (argc, argv, "t:z:i:o:n:s:w:e:d:pcgh")) != -1)
        switch (argument)
        {
            case 't':
                config->tolerance = atof(optarg);
                break;
            case 'z':
                config->requested_size = (size_t)atoi(optarg);
                break;
            case 'o':
                config->output_filename = optarg;
                break;
            case 'i':
                config->input_filename = optarg;
                config->read_from_ASC = true;
                break;
            case 'n':
                config->north_border = atof(optarg);
                config->read_from_ASC = false;
                break;
            case 's':
                config->south_border = atof(optarg);
                config->read_from_ASC = false;
                break;
            case 'w':
                config->west_border = atof(optarg);
                config->read_from_ASC = false;
                break;
            case 'e':
                config->east_border = atof(optarg);
                config->read_from_ASC = false;
                break;
            case 'd':
                config->map_dir = optarg;
                config->read_from_ASC = false;
                break;
            case 'p':
                config->use_inp = true;
                break;
            case 'g':
                config->post_utm = false;
                break;
            case 'c':
                config->pre_utm = true;
                config->post_utm = false;
                break;
            case 'h':
                config->use_height = true;
                break;
            case '?':
                if (optopt == 't' || optopt == 'z')
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
                exit(1);
            default:
                abort();
        }
}
