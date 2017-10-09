//
// Created by krzysiek on 09.10.17.
//

#ifndef MESHGEN_INPUT_H
#define MESHGEN_INPUT_H

#define RESOLUTION 3
static const int VALUES_IN_DEGREE = 60 * 60 / RESOLUTION;
static const unsigned short PIXEL_SIZE = 2;

short **read_map(double begin_longitude, double begin_latitude, double end_longitude, double end_latitude,
                 char *map_dir);
short **init_map(int length, int width);
char *get_filename(const char *map_dir, int begin_longitude_int, int begin_latitude_int);
char *concatenate(const char *e1, const char *e2, const char *e3, const char *e4);
short **read_map2(const char *map_dir, int begin_longitude_int, int begin_latitude_int,
                  unsigned int width, unsigned int length);
void swap_if_needed(int *should_be_lower, int *should_be_bigger);
#endif //MESHGEN_INPUT_H
