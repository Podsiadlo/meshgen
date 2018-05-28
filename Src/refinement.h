#ifndef MESHGEN_REFINEMENT_H
#define MESHGEN_REFINEMENT_H

static const int STEP_IN_METERS = 90;

#include <stdbool.h>

#include "mesh.h"
#include "triangle.h"

bool
refine_if_required(struct triangle *triangle, double tolerance, struct mesh *mesh);

int
refine(struct triangle *triangle, struct mesh *mesh);

bool
is_final_step(struct triangle *triangle, struct mesh *mesh);

bool
inside_condition(const struct triangle *triangle, double tolerance, struct mesh *mesh);

bool
outside_condition(struct triangle *triangle, double tolerance, struct mesh *mesh);

bool
split(struct triangle *triangle, struct mesh *mesh);

void
split_inner(struct triangle *triangle1, struct triangle *triangle2,
            struct mesh *mesh);

void
split_border(struct triangle *triangle, struct mesh *mesh);

#endif // MESHGEN_REFINEMENT_H
