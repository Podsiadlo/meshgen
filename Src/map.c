#include "map.h"

struct map *
init_map(const double ** data, size_t width, size_t length, double cell_width, double cell_length)
{
    struct map *map = (struct map *)malloc(sizeof(struct map));
    map->data = data;
    map->width = width;
    map->length = length;
    map->cell_width = cell_width;
    map->cell_length = cell_length;
    return map;
}

double **
init_map_data(size_t rows, size_t cols)
{
    double **map;
    map = (double **)malloc(rows * sizeof(double *));
    for (size_t i = 0; i < rows; ++i) {
        map[i] = (double *)malloc(cols * sizeof(double));
    }
    return map;
}

void
free_map(struct map *map)
{
    for (size_t i = 0; i < map->length; ++i) {
        free((double *) map->data[i]);
    }
    free(map->data);
    free(map);
}
