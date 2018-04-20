#include "input.h"

#include <errno.h>
#include <math.h>
#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "asc_reader.h"

double **
read_map(const double begin_longitude, const double begin_latitude,
         const double end_longitude, const double end_latitude, char *map_dir)
{ // map[row][column] - it's array of rows
    int begin_longitude_int = (int)round(
        begin_longitude *
        VALUES_IN_DEGREE); // Rounding to avoid problems with numerical errors
    int begin_latitude_int = (int)round(begin_latitude * VALUES_IN_DEGREE);
    int end_longitude_int = (int)round(end_longitude * VALUES_IN_DEGREE);
    int end_latitude_int = (int)round(end_latitude * VALUES_IN_DEGREE);

    swap_if_needed(&begin_latitude_int, &end_latitude_int);
    swap_if_needed(&begin_longitude_int, &end_longitude_int);

    unsigned int width = end_longitude_int - begin_longitude_int;
    unsigned int length = end_latitude_int - begin_latitude_int;

    double **map = read_map2(map_dir, begin_longitude_int, begin_latitude_int,
                            width, length);

    return map;
}

double **
read_map2(const char *map_dir, int begin_longitude_int, int begin_latitude_int,
          unsigned int width, unsigned int length)
{
    double **map = init_map(length, width);
    char file_to_open[14];

    get_filename(file_to_open, map_dir, begin_longitude_int,
                 begin_latitude_int);

    FILE *map_file;
    if ((map_file = fopen(file_to_open, "rb")) == NULL) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }
    int rows = VALUES_IN_DEGREE + 1;
    if (fseek(map_file,
              ((rows - (begin_latitude_int % rows)) * rows +
               (begin_longitude_int % rows)) *
                  PIXEL_SIZE,
              SEEK_SET) == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }
    uint16_t* buffer = (uint16_t *)malloc(PIXEL_SIZE*width);
    for (size_t i = 0; i < length; ++i) {
        fread(buffer, PIXEL_SIZE, width, map_file);
        if (fseek(map_file, (rows - width) * PIXEL_SIZE, SEEK_CUR) == -1) {
            fprintf(stderr, "%s\n", strerror(errno));
            exit(1);
        }
        for (size_t j = 0; j < width; ++j) {
            uint16_t tmp = buffer[j];
            tmp <<= 8;
            buffer[j] >>= 8;
            buffer[j] |= tmp;
            map[length - 1 - i][j] = buffer[j];
        }
    }
    free(buffer);
    if (fclose(map_file) != 0) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }
    return map;
}

void
free_map(double **map, int length)
{
    for (int i = 0; i < length; ++i) {
        free(map[i]);
    }
    free(map);
}

void
swap_if_needed(int *should_be_lower, int *should_be_bigger)
{
    if ((*should_be_lower) > (*should_be_bigger)) {
        int tmp = (*should_be_lower);
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

double **
init_map(int length, int width)
{
    double **map;
    map = (double **)malloc(length * sizeof(double *));
    for (int i = 0; i < length; ++i) {
        map[i] = (double *)malloc(width * sizeof(double));
    }
    return map;
}
