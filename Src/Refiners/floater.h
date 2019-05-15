#ifndef MESHGEN_FLOATER_H
#define MESHGEN_FLOATER_H

#include <vector>

void floater(struct Face * faces, int faces_size);

void compute_normal(struct point * node);

std::vector scalar_product(struct point *point, std::vector<double> &vector);

double product(std::vector v1, std::vector v2);

std::vector substract(std::vector &a, std::vector &b);

double length(std::vector<double> &v);

#endif //MESHGEN_FLOATER_H
