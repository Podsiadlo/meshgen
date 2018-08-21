#ifndef MESHGEN_MAP_H
#define MESHGEN_MAP_H

#include <stdlib.h>
#include <stdbool.h>

struct map {
    size_t width;
    size_t length;
    double cell_width;
    double cell_length;
    const double **data;
    double north_border;
    double west_border;
    bool utm; //TODO: Consider moving it to some better place
    long zone;
    char hemisphere;
};

struct map *
init_map(const double ** data, size_t width, size_t length, double cell_width, double cell_length);

double **
init_map_data(size_t rows, size_t cols);

void
print_map(struct map* map);

void
free_map(struct map *map);

#endif //MESHGEN_MAP_H
