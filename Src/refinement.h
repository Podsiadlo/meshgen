#ifndef MESHGEN_REFINEMENT_H
#define MESHGEN_REFINEMENT_H

#include <stdbool.h>

#include "mesh.h"
#include "triangle.h"

bool
refine_if_required(struct triangle *triangle, double tolerance, struct mesh *mesh, bool use_height);

int
refine(struct triangle *triangle, struct mesh *mesh, bool use_height);

bool
is_final_step(struct triangle *triangle, struct mesh *mesh);

bool
inside_condition(const struct triangle *triangle, double tolerance, struct mesh *mesh);

bool
outside_condition(struct triangle *triangle, double tolerance, struct mesh *mesh);

bool
split(struct triangle *triangle, struct mesh *mesh, bool use_height);

void
split_inner(struct triangle *triangle1, struct triangle *triangle2, struct mesh *mesh, bool use_height);

void
split_border(struct triangle *triangle, struct mesh *mesh, bool use_height);

#endif // MESHGEN_REFINEMENT_H
