#ifndef MESHGEN_INPUT_H
#define MESHGEN_INPUT_H

#include <stdlib.h>
#define RESOLUTION 3
static const int VALUES_IN_DEGREE = 60 * 60 / RESOLUTION;
static const unsigned short PIXEL_SIZE = 2;

double **
read_map(const double begin_longitude, const double begin_latitude, const double end_longitude,
         const double end_latitude, char *map_dir);

double **
read_map2(const char *map_dir, int begin_longitude_int, int begin_latitude_int,
          size_t cols, size_t rows);

double **
init_map(size_t rows, size_t cols);

void
get_filename(char *filename, const char *map_dir, int begin_longitude_int,
             int begin_latitude_int);

void
swap_if_needed(int *should_be_lower, int *should_be_bigger);

void
free_map(double **map, size_t rows);

#endif // MESHGEN_INPUT_H
