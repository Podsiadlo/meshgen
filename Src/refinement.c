#include "refinement.h"

#include <math.h>
#include <stdlib.h>

bool
would_create_flat_triangle(struct triangle *triangle);

bool
refine_if_required(struct triangle *triangle, struct mesh *mesh)
{
    bool refined = false;
    if ((inside_condition(triangle, mesh) ||
         outside_condition(triangle, mesh)) /*&& !is_too_small(triangle)*/) {
        if (refine(triangle, mesh) > 0) {
            refined = true;
        }
    }
    return refined;
}

bool
would_create_flat_triangle(struct triangle *triangle)
{
    short longest = triangle->longest;
    struct point center;
    get_longest_edge_midsection(&center, triangle);
    if ((center.x == triangle->vertices[(longest + 2) % 3].x && //helper
         (center.x == triangle->vertices[longest].x ||
          center.x == triangle->vertices[(longest + 1) % 3].x)) ||
        (center.y == triangle->vertices[(longest + 2) % 3].y &&
         (center.y == triangle->vertices[longest].y ||
          center.y == triangle->vertices[(longest + 1) % 3].y))) {

        return true;
    }

    return false;
}

bool
inside_condition(const struct triangle *triangle, struct mesh *mesh)
{
    return fabs(get_height_mean(triangle) -
                get_height_of_center(triangle, mesh->map)) > EPSILON;
}

bool
outside_condition(struct triangle *triangle, struct mesh *mesh)
{
    short center = get_height_of_center(triangle, mesh->map);
    int delta;
    for (int i = 0; i < 3; ++i) {
        if (triangle->neighbours[i] != -1) {
            delta = abs(
                abs(triangle->vertices[i].z -
                    triangle->vertices[(i + 1) % 3].z) -
                abs(get_height_of_center(
                        get_triangle(triangle->neighbours[i], mesh->triangles), mesh->map) -
                    center));
            if (delta > EPSILON) {
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
    short longest = triangle->longest;
    if (abs(triangle->vertices[longest].x -
            triangle->vertices[(1 + longest) % 3].x) <= 1 &&
        abs(triangle->vertices[longest].y -
            triangle->vertices[(1 + longest) % 3].y) <= 1) {

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
    init_triangle(new_triangle, triangle->vertices[(longest + 2) % 3].x,
                  triangle->vertices[(longest + 2) % 3].y,
                  triangle->vertices[longest].x, triangle->vertices[longest].y,
                  center.x, center.y, mesh->map);
    new_triangle->neighbours[0] = triangle->neighbours[(longest + 2) % 3];
    new_triangle->neighbours[1] = -1;
    new_triangle->neighbours[2] = triangle->index;

    // Fix old triangle
    triangle->vertices[longest].x = center.x;
    triangle->vertices[longest].y = center.y;
    triangle->vertices[longest].z =
        mesh->map[triangle->vertices[longest].y][triangle->vertices[longest].x];
    triangle->neighbours[(longest + 2) % 3] = new_triangle->index;

    // Fix neighbour
    struct triangle *neighbour = get_triangle(new_triangle->neighbours[0], mesh->triangles); //wczesniej to przesunac
    if (neighbour != NULL) {
        if (neighbour->neighbours[0] == triangle->index) {//is it possible to happen?
            neighbour->neighbours[0] = new_triangle->index;
        } else if (neighbour->neighbours[1] == triangle->index) {
            neighbour->neighbours[1] = new_triangle->index;
        } else {
            neighbour->neighbours[2] = new_triangle->index;
        }
    }

    fix_longest(triangle);
    fix_longest(new_triangle);
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
    center.z = mesh->map[center.y][center.x];

    struct point *points[4];
    struct triangle *neighbours[4];
    struct triangle *triangles[4];
    int outside_borders[4];

    // Simplify symbols
    points[0] = &(triangle1->vertices[(triangle1->longest + 1) % 3]);
    points[1] = &(triangle1->vertices[(triangle1->longest + 2) % 3]);
    points[2] = &(triangle1->vertices[triangle1->longest]);
    neighbours[0] =
            get_triangle(triangle1->neighbours[(triangle1->longest + 1) % 3], mesh->triangles);
    neighbours[1] =
            get_triangle(triangle1->neighbours[(triangle1->longest + 2) % 3], mesh->triangles);
    outside_borders[0] = (triangle1->longest + 1) % 3;

    points[3] = &(triangle2->vertices[(triangle2->longest + 2) % 3]);
    points[0] = &(triangle2->vertices[triangle2->longest]);
    neighbours[3] =
            get_triangle(triangle2->neighbours[(triangle2->longest + 2) % 3], mesh->triangles);
    neighbours[2] =
            get_triangle(triangle2->neighbours[(triangle2->longest + 1) % 3], mesh->triangles);
    outside_borders[2] = (triangle2->longest + 1) % 3;

    triangles[0] = triangle1;
    triangles[1] = get_new_triangle(mesh);
    init_triangle(triangles[1], points[1]->x, points[1]->y, points[2]->x,
                  points[2]->y, center.x, center.y, mesh->map);
    outside_borders[1] = 0;
    triangles[2] = triangle2;
    triangles[3] = get_new_triangle(mesh);
    init_triangle(triangles[3], points[3]->x, points[3]->y, points[0]->x,
                  points[0]->y, center.x, center.y, mesh->map);
    outside_borders[3] = 0;

    // Modify input triangles
    points[0]->x = center.x;
    points[0]->y = center.y;
    points[0]->z = center.z;
    //    points[0] = &(triangles[3]->vertices[1]); //useless, but I leave it to
    //    indicate that they need to be set before using
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
