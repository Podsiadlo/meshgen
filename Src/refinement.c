#include "refinement.h"

#include <math.h>
#include <stdlib.h>


bool
refine_if_required(struct triangle *triangle, struct mesh *mesh)
{
    bool refined = false;
    if (inside_condition(triangle, mesh) /*||
         outside_condition(triangle, mesh)*/) {
        if (refine(triangle, mesh) > 0) {
            refined = true;
        }
    }
    return refined;
}

bool
would_create_flat_triangle(struct triangle *triangle) //TODO: Check if it's still required
{
    struct point center;
    get_longest_edge_midsection(&center, triangle);
    if ((center.x == get_opposite_vertex(triangle)->x &&
         (center.x == get_1st_longest_edge_vertex(triangle)->x ||
          center.x == get_2nd_longest_edge_vertex(triangle)->x)) ||
        (center.y == get_opposite_vertex(triangle)->y &&
         (center.y == get_1st_longest_edge_vertex(triangle)->y ||
          center.y == get_2nd_longest_edge_vertex(triangle)->y))) {

        return true;
    }

    return false;
}

bool
inside_condition(const struct triangle *triangle, struct mesh *mesh) //TODO: optimize
{
    double height_mean = get_height_mean(triangle);
    double lowest_x =
            triangle->vertices[0].x < triangle->vertices[1].x ? triangle->vertices[0].x : triangle->vertices[1].x;
    lowest_x = triangle->vertices[2].x < lowest_x ? triangle->vertices[2].x : lowest_x;
    double highest_x =
            triangle->vertices[0].x > triangle->vertices[1].x ? triangle->vertices[0].x : triangle->vertices[1].x;
    highest_x = triangle->vertices[2].x > highest_x ? triangle->vertices[2].x : highest_x;
    double lowest_y =
            triangle->vertices[0].y < triangle->vertices[1].y ? triangle->vertices[0].y : triangle->vertices[1].y;
    lowest_y = triangle->vertices[2].y < lowest_y ? triangle->vertices[2].y : lowest_y;
    double highest_y =
            triangle->vertices[0].y > triangle->vertices[1].y ? triangle->vertices[0].y : triangle->vertices[1].y;
    highest_y = triangle->vertices[2].y > highest_y ? triangle->vertices[2].y : highest_y;

    for (int i = (int)floor(lowest_x); i < ceil(highest_x); ++i) {
        for (int j = (int)floor(lowest_y); j < ceil(highest_y); ++j) {
            if (is_inside_triangle(i, j, triangle)) {
                if (fabs(height_mean - get_height(i, j, mesh->map)) > TOLERANCE) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool
outside_condition(struct triangle *triangle, struct mesh *mesh)
{
    double center = get_height_of_center(triangle, mesh->map);
    double delta;
    for (int i = 0; i < 3; ++i) {
        if (triangle->neighbours[i] != -1) {
            delta = fabs(
                fabs(triangle->vertices[i].z -
                    triangle->vertices[(i + 1) % 3].z) -
                fabs(get_height_of_center(
                        get_triangle(triangle->neighbours[i], mesh->triangles), mesh->map) -
                    center));
            if (delta > TOLERANCE) {
                return true;
            }
        }
    }
    return false;
}

int
refine(struct triangle *triangle, struct mesh *mesh)
{
    if (is_too_small(triangle)) {
        return 0;
    }
    if (is_final_step(triangle, mesh)) {
        return split(triangle, mesh) ? 1 : 0;
    } else {
        int refinements =
            refine(get_triangle(get_longest_edge_triangle_index(triangle), mesh->triangles), mesh);
        refinements += split(triangle, mesh) ? 1 : 0;
        return refinements;
    }
}

bool
is_too_small(struct triangle *triangle)
{
    if (fabs(get_1st_longest_edge_vertex(triangle)->x -
            get_2nd_longest_edge_vertex(triangle)->x) <= 1 &&
        fabs(get_1st_longest_edge_vertex(triangle)->y -
            get_2nd_longest_edge_vertex(triangle)->y) <= 1) {

        return true;
    }

    if (would_create_flat_triangle(triangle)) {
        return true;
    }

    return false;
}

bool
is_final_step(struct triangle *triangle, struct mesh *mesh)
{
    int next_triangle = get_longest_edge_triangle_index(triangle);
    return next_triangle == -1 ||
            get_longest_edge_triangle_index(get_triangle(next_triangle, mesh->triangles)) ==
        triangle->index;
}

bool
split(struct triangle *triangle, struct mesh *mesh)
{
    struct triangle *next_triangle =
            get_triangle(get_longest_edge_triangle_index(triangle), mesh->triangles);
    if (next_triangle == NULL) {
        split_border(triangle, mesh);
        return true;
    } else if (!would_create_flat_triangle(next_triangle)) {
        split_inner(triangle, next_triangle, mesh);
        return true;
    } else {
        return false;
    }
}

void
split_border(struct triangle *triangle, struct mesh *mesh)
{
    struct point center;
    get_longest_edge_midsection(&center, triangle);
    struct triangle *new_triangle;
    short longest = triangle->longest;

    // Create new triangle
    new_triangle = get_new_triangle(mesh);
    init_triangle(new_triangle, get_opposite_vertex(triangle)->x,
                  get_opposite_vertex(triangle)->y,
                  get_1st_longest_edge_vertex(triangle)->x, get_1st_longest_edge_vertex(triangle)->y,
                  center.x, center.y, mesh->map);
    new_triangle->neighbours[0] = get_2nd_shorter_edge_triangle_index(triangle);
    new_triangle->neighbours[1] = -1;
    new_triangle->neighbours[2] = triangle->index;
    struct triangle *neighbour = get_triangle(new_triangle->neighbours[0], mesh->triangles);

    // Fix old triangle
    get_1st_longest_edge_vertex(triangle)->x = center.x;
    get_1st_longest_edge_vertex(triangle)->y = center.y;
    get_1st_longest_edge_vertex(triangle)->z = get_height(center.x, center.y, mesh->map);
    triangle->neighbours[(longest + 2) % 3] = new_triangle->index;
    fix_longest(triangle);

    // Fix neighbour
    if (neighbour != NULL) {
        if (neighbour->neighbours[0] == triangle->index) {//is it possible to happen?
            neighbour->neighbours[0] = new_triangle->index;
        } else if (neighbour->neighbours[1] == triangle->index) {
            neighbour->neighbours[1] = new_triangle->index;
        } else {
            neighbour->neighbours[2] = new_triangle->index;
        }
    }

#ifndef NDEBUG
    verify_triangle(triangle, mesh);
    verify_triangle(new_triangle, mesh);
#endif
}

void
split_inner(struct triangle *triangle1, struct triangle *triangle2,
            struct mesh *mesh)
{
    struct point center;
    get_longest_edge_midsection(&center, triangle1);
    center.z = get_height(center.x, center.y, mesh->map);

    struct point *points[4];
    struct triangle *neighbours[4];
    struct triangle *triangles[4];
    int outside_borders[4];
    triangles[1] = get_new_triangle(mesh);
    triangles[3] = get_new_triangle(mesh);
    //in case it was reallocated during triangles[3] creation
    //FIXME: In fact, it can dangerous, as now triangles[1]
    //might be pointing to memory with 'undefined' values
    triangles[0] = get_triangle(triangle1->index, mesh->triangles);
    triangles[1] = get_triangle(triangles[1]->index, mesh->triangles);
    triangles[2] = get_triangle(triangle2->index, mesh->triangles);

    // Simplify symbols
    points[0] = get_2nd_longest_edge_vertex(triangles[0]);
    points[1] = get_opposite_vertex(triangles[0]);
    points[2] = get_1st_longest_edge_vertex(triangles[0]);
    neighbours[0] = get_triangle(get_1st_shorter_edge_triangle_index(triangles[0]), mesh->triangles);
    neighbours[1] = get_triangle(get_2nd_shorter_edge_triangle_index(triangles[0]), mesh->triangles);
    outside_borders[0] = (triangles[0]->longest + 1) % 3;

    points[3] = get_opposite_vertex(triangles[2]);
    points[0] = get_1st_longest_edge_vertex(triangles[2]);
    neighbours[3] =
            get_triangle(get_2nd_shorter_edge_triangle_index(triangles[2]), mesh->triangles);
    neighbours[2] =
            get_triangle(get_1st_shorter_edge_triangle_index(triangles[2]), mesh->triangles);
    outside_borders[2] = (triangles[2]->longest + 1) % 3;

    init_triangle(triangles[1], points[1]->x, points[1]->y, points[2]->x,
                  points[2]->y, center.x, center.y, mesh->map);
    outside_borders[1] = 0;
    init_triangle(triangles[3], points[3]->x, points[3]->y, points[0]->x,
                  points[0]->y, center.x, center.y, mesh->map);
    outside_borders[3] = 0;

    // Modify input triangles
    points[0]->x = center.x;
    points[0]->y = center.y;
    points[0]->z = center.z;
    //    points[0] = &(triangles[3]->vertices[1]); //useless, but I'm leaving it to
    //    indicate that they need to be set before using points[0] again
    points[2]->x = center.x;
    points[2]->y = center.y;
    points[2]->z = center.z;
    //    points[2] = &(triangles[1]->vertices[1]);

    // Set neighbours
    for (int k = 0; k < 4; ++k) {
        triangles[k]->neighbours[outside_borders[k]] =
            neighbours[k] != NULL ? neighbours[k]->index : -1;
        triangles[k]->neighbours[(outside_borders[k] + 1) % 3] =
            triangles[(k + 1) % 4]->index;
        triangles[k]->neighbours[(outside_borders[k] + 2) % 3] =
            triangles[(k + 3) % 4]->index;
        fix_longest(triangles[k]);
    }

    // Configure neighbours
    for (int i = 0; i < 4; ++i) {
        if (neighbours[i] != NULL) {
            if (neighbours[i]->neighbours[0] == triangles[(i / 2) * 2]->index) {
                neighbours[i]->neighbours[0] = triangles[i]->index;
            } else if (neighbours[i]->neighbours[1] ==
                       triangles[(i / 2) * 2]->index) {
                neighbours[i]->neighbours[1] = triangles[i]->index;
            } else if (neighbours[i]->neighbours[2] ==
                       triangles[(i / 2) * 2]->index) {
                neighbours[i]->neighbours[2] = triangles[i]->index;
            }
        }
    }
#ifndef NDEBUG
    for (int j = 0; j < 4; ++j) {
        verify_triangle(triangles[j], mesh);
    }
#endif
}
