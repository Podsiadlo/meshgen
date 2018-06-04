#include "input.h"
#include "map.h"

#include <errno.h>
#include <math.h>
#include <memory.h>
#include <stdint.h>
#include <stdio.h>

struct map *
read_map(const double west_border, const double north_border,
         const double east_border, const double south_border, char *map_dir)
{ // data[row][column] - it's array of rows

    swap_if_needed((double *) &north_border, (double *) &south_border);
    swap_if_needed((double *) &west_border, (double *) &east_border);
    // Rounding to avoid problems with numerical errors
    int west_border_int = (int)round(west_border * VALUES_IN_DEGREE);
    int north_border_int = (int)round(north_border * VALUES_IN_DEGREE);
    int east_border_int = (int)round(east_border * VALUES_IN_DEGREE);
    int south_border_int = (int)round(south_border * VALUES_IN_DEGREE);


    size_t cols = (size_t) (east_border_int - west_border_int);
    size_t rows = (size_t) (south_border_int - north_border_int);
    double **map_data = init_map_data(rows, cols);
    struct map * map = (struct map*)malloc(sizeof(struct map));
    map->data = (const double **) map_data;
    map->length = rows;
    map->width = cols;
    map->cell_length = 1. / VALUES_IN_DEGREE;
    map->cell_width = 1. / VALUES_IN_DEGREE;
    map->north_border = north_border;
    map->west_border = west_border;

    read_map2(map_data, map_dir, west_border_int, north_border_int, cols, rows);

    return map;
}

void
read_map2(double **map_data, const char *map_dir, int west_border_int, int north_border_int,
          size_t cols, size_t rows)
{
    char file_to_open[256];

    get_filename(file_to_open, map_dir, west_border_int, north_border_int);

    FILE *map_file;
    if ((map_file = fopen(file_to_open, "rb")) == NULL) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }
    int rows_in_degree = VALUES_IN_DEGREE + 1;
    if (fseek(map_file,
              ((rows_in_degree - (north_border_int % rows_in_degree)) * rows_in_degree +
               (west_border_int % rows_in_degree)) *
                  PIXEL_SIZE,
              SEEK_SET) == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }
    uint16_t* buffer = (uint16_t *)malloc(PIXEL_SIZE*cols);
    for (size_t i = 0; i < rows; ++i) {
        fread(buffer, PIXEL_SIZE, cols, map_file);
        if (fseek(map_file, (rows_in_degree - cols) * PIXEL_SIZE, SEEK_CUR) == -1) {
            fprintf(stderr, "%s\n", strerror(errno));
            exit(1);
        }
        for (size_t j = 0; j < cols; ++j) {
            uint16_t tmp = buffer[j];
            tmp <<= 8;
            buffer[j] >>= 8;
            buffer[j] |= tmp;
            map_data[rows - 1 - i][j] = buffer[j];
        }
    }
    free(buffer);
    if (fclose(map_file) != 0) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }
}

void
swap_if_needed(double *should_be_lower, double *should_be_bigger)
{
    if ((*should_be_lower) > (*should_be_bigger)) {
        double tmp = (*should_be_lower);
        (*should_be_lower) = (*should_be_bigger);
        (*should_be_bigger) = tmp;
    }
}

void
get_filename(char *filename, const char *map_dir, int begin_longitude_int,
             int begin_latitude_int)
{
    int first_long_to_read;
    int first_lat_to_read;

    if (begin_longitude_int < 0) {
        if (begin_longitude_int % VALUES_IN_DEGREE != 0) {
            first_long_to_read = begin_longitude_int / VALUES_IN_DEGREE + 1;
        } else {
            first_long_to_read = begin_longitude_int / VALUES_IN_DEGREE;
        }
    } else {
        first_long_to_read = begin_longitude_int / VALUES_IN_DEGREE;
    }

    if (begin_latitude_int < 0) {
        if (begin_latitude_int % VALUES_IN_DEGREE != 0) {
            first_lat_to_read = begin_latitude_int / VALUES_IN_DEGREE + 1;
        } else {
            first_lat_to_read = begin_latitude_int / VALUES_IN_DEGREE;
        }
    } else {
        first_lat_to_read = begin_latitude_int / VALUES_IN_DEGREE;
    }

    sprintf(filename, "%s/%s%d%s%.3d.hgt", map_dir,
            first_long_to_read < 0 ? "S" : "N", first_long_to_read,
            first_lat_to_read < 0 ? "W" : "E", first_lat_to_read);
}

