#ifndef MESHGEN_REFINEMENT_H
#define MESHGEN_REFINEMENT_H

#include <stdbool.h>
#include "triangle.h"
#include "mesh.h"

bool is_final_step(struct triangle *triangle);

void refine(struct triangle *triangle, struct mesh *mesh);

void split(struct triangle *triangle, struct mesh *mesh);

void split_inner(struct triangle *triangle1, struct triangle *triangle2, struct mesh *mesh);

void split_border(struct triangle *triangle, struct mesh *mesh);

bool refine_if_required(struct triangle *triangle, struct mesh *mesh);

bool inside_condition(const struct triangle *triangle, const short **map);

bool outside_condition(struct triangle *triangle, const short **map);

#endif //MESHGEN_REFINEMENT_H
