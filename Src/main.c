#include "Readers/srtm_reader.h"
#include "Readers/asc_reader.h"
#include "DataStructures/mesh.h"
#include "Writers/writer.h"
#include "Refiners/refinement.h"
#include "Utils/utils.h"

#include <getopt.h>
#include <ctype.h>
#include <string.h>

static char *const USAGE = "OPTIONS:\n"
                        "\t-t <tolerance>\n"
                        "\t-s <requsted_size>\n"
                        "\t-i <data_file>\n"
                        "\t-o <output_file>\n"
                        "\t-d <data_dir>\n"
                        "\t-N <north_border>\n"
                        "\t-S <south_border>\n"
                        "\t-W <west_border>\n"
                        "\t-E <east_border>\n"
                        "\t-p (to write result into .inp file)\n"
                        "\t-m (to write result into .smesh file)\n"
                        "\t-g (to skip conversion to UTM)\n"
                        "\t-u (to convert to UTM before refinement)\n"
                        "\t-U (to convert to UTM after refinement)\n"
                        "\t-h (to consider height of points during refinement)\n"
                        "\t-f (to use Floater algorithm)\n"
                        "\n"
                        "You probably want to run it with either -p or -m.\n";

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
        map = read_ASC(config->input_filename);
    } else {
        map = read_SRTM(config->west_border, config->north_border, config->east_border, config->south_border,
                        config->map_dir);
    }
//    print_map(map);

    //Actual algorithm
    struct mesh* mesh = generate_mesh(map, config->requested_size, config->use_height);
    if (config->pre_utm) {
        convert_mesh_to_UTM(mesh);
    }
    refine_mesh(mesh, config->tolerance, config->use_height);

    //writing results
    char buffer[256];
    if (config->use_inp) {
        strcpy(buffer, config->output_filename);
        save_to_inp(mesh, strcat(buffer, ".inp"), config->post_utm);
    }
    if (config->use_smesh) {
        strcpy(buffer, config->output_filename);//This line is duplicated to avoid making files with multiple extensions
        save_to_smesh(mesh, strcat(buffer, ".smesh"), config->pre_utm, config->post_utm);
    }

    //cleaning memory
    free_mesh(mesh);
    free(config);

    return 0;
}

void set_default_config(struct config *config) {
    config->tolerance = 10;
    config->requested_size = SIZE_MAX;
    config->output_filename = "out/d6";
    config->input_filename = "Examples/test1.asc";
    config->read_from_ASC = false;
    config->west_border = 19.6;
    config->north_border = 49.35;
    config->east_border = 20.3;
    config->south_border = 49.1;
    config->map_dir = "Data";
    config->use_inp = false;
    config->use_smesh = false;
    config->post_utm = true;
    config->pre_utm = false;
    config->use_height = false;
    config->use_floater = false;
}

void
parse_arguments(int argc, char **argv, struct config * config)
{
    int argument;
    if (argc == 1) {
        fprintf(stderr, USAGE);
    }
    while ((argument = getopt (argc, argv, "t:s:i:o:N:S:W:E:d:pgUuhmf")) != -1)
        switch (argument)
        {
            case 't':
                config->tolerance = atof(optarg);
                break;
            case 's': {
                double s = atof(optarg);
                config->requested_size = (size_t) s * VALUES_IN_DEGREE;
            }
                break;
            case 'o':
                config->output_filename = optarg;
                break;
            case 'i':
                config->input_filename = optarg;
                config->read_from_ASC = true;
                break;
            case 'N':
                config->north_border = atof(optarg);
                config->read_from_ASC = false;
                break;
            case 'S':
                config->south_border = atof(optarg);
                config->read_from_ASC = false;
                break;
            case 'W':
                config->west_border = atof(optarg);
                config->read_from_ASC = false;
                break;
            case 'E':
                config->east_border = atof(optarg);
                config->read_from_ASC = false;
                break;
            case 'd':
                config->map_dir = optarg;
                config->read_from_ASC = false;
                break;
            case 'g':
                config->post_utm = false;
                config->pre_utm = false;
                break;
            case 'u':
                config->pre_utm = true;
                config->post_utm = false;
                break;
            case 'U':
                config->pre_utm = false;
                config->post_utm = true;
                break;
            case 'h':
                config->use_height = true;
                break;
            case 'f':
                config->use_floater = true;
                break;
            case 'm':
                config->use_smesh = true;
                break;
            case 'p':
                config->use_inp = true;
                break;
            case '?':
                if (optopt == 't' || optopt == 's' || optopt == 'i' || optopt == 'o' || optopt == 'd'
                    || optopt == 'N' || optopt == 'S' || optopt == 'W' || optopt == 'E') {

                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                    fprintf(stderr, USAGE);
                } else if (isprint (optopt)) {
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                    fprintf(stderr, USAGE);
                } else {
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                    fprintf(stderr, USAGE);
                }
                exit(1);
            default:
                abort();
        }
}
