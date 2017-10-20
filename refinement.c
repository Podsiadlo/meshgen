#include <math.h>
#include <stdlib.h>
#include "refinement.h"


bool is_too_small(struct triangle *triangle);

bool refine_if_required(struct triangle *triangle, struct mesh *mesh) {
    bool refined = false;
    if (inside_condition(triangle, mesh) || outside_condition(triangle, mesh) || !is_too_small(triangle)) {
        refine(triangle, mesh);
        refined = true;
    }
    return refined;
}

bool is_too_small(struct triangle *triangle) {
    if (abs(triangle->vertices[triangle->longest].x - triangle->vertices[(1 + triangle->longest) % 3].x) <= 1
        && abs(triangle->vertices[triangle->longest].y - triangle->vertices[(1 + triangle->longest) % 3].y) <= 1) {

        return true;
    }
    return false;
}

bool inside_condition(const struct triangle *triangle, struct mesh *mesh) {
    return fabs(get_height_mean(triangle) - get_height_of_center(triangle, mesh->map)) > EPSILON;
}

bool outside_condition(struct triangle *triangle, struct mesh *mesh) {
    short center = get_height_of_center(triangle, mesh->map);
    int delta;
    for (int i = 0; i < 3; ++i) {
        if (triangle->children[i] != -1) {
            delta = abs(abs(triangle->vertices[i].z - triangle->vertices[(i + 1) % 3].z) -
                         abs(get_height_of_center(get_triangle(triangle->children[i], mesh), mesh->map) - center));
            if (delta > EPSILON) {
                return true;
            }
        }
    }
    return false;
}

void refine(struct triangle *triangle, struct mesh *mesh) {
    if (is_final_step(triangle, mesh)) {
        split(triangle, mesh);
    } else {
        refine(get_triangle(get_next_triangle_index(triangle), mesh), mesh);
    }
}

bool is_final_step(struct triangle *triangle, struct mesh *mesh) {
    int next_triangle = get_next_triangle_index(triangle);
    return next_triangle == -1 || get_next_triangle_index(get_triangle(next_triangle, mesh)) == triangle->index;
}

void split(struct triangle *triangle, struct mesh *mesh) {
    struct triangle *next_triangle = get_triangle(get_next_triangle_index(triangle), mesh);
    if (next_triangle == NULL) {
        split_border(triangle, mesh);
    } else {
        split_inner(triangle, next_triangle, mesh);
    };
}


void split_border(struct triangle *triangle, struct mesh *mesh) {
    struct point center;
    get_longest_edge_midsection(&center, triangle);
    struct triangle *new_triangle;
    short longest = triangle->longest;

    //Create new triangle
    new_triangle = get_new_triangle(mesh);
    init_triangle(new_triangle, triangle->vertices[(longest + 2) % 3].x, triangle->vertices[(longest + 2) % 3].y,
                  triangle->vertices[longest].x, triangle->vertices[longest].y,
                  center.x, center.y, mesh->map);
    new_triangle->children[0] = triangle->children[(longest + 2) % 3];
    new_triangle->children[1] = -1;
    new_triangle->children[2] = triangle->index;

    //Fix old triangle
    triangle->vertices[longest].x = center.x;
    triangle->vertices[longest].y = center.y;
    triangle->vertices[longest].z = mesh->map[triangle->vertices[longest].y][triangle->vertices[longest].x];
    triangle->children[(longest + 2) % 3] = new_triangle->index;

    //Fix neighbour
    struct triangle *neighbour = get_triangle(new_triangle->children[0], mesh);
    if (neighbour != NULL) {
        if (neighbour->children[0] == triangle->index) {
            neighbour->children[0] = new_triangle->index;
        } else if (neighbour->children[1] == triangle->index) {
            neighbour->children[1] = new_triangle->index;
        } else {
            neighbour->children[2] = new_triangle->index;
        }
    }

    fix_longest(triangle);
    fix_longest(new_triangle);

    verify_neighbours(triangle, mesh);
    verify_neighbours(new_triangle, mesh);
}


void split_inner(struct triangle *triangle1, struct triangle *triangle2, struct mesh *mesh) {
    struct point center;
    get_longest_edge_midsection(&center, triangle1);
    center.z = mesh->map[center.y][center.x];

    struct point *points[4];
    struct triangle *children[4];
    struct triangle *triangles[4];
    short outside_borders[4];

    //Simplify symbols
    points[0] = &(triangle1->vertices[(triangle1->longest + 1) % 3]);
    points[1] = &(triangle1->vertices[(triangle1->longest + 2) % 3]);
    points[2] = &(triangle1->vertices[triangle1->longest]);
    children[0] = get_triangle(triangle1->children[(triangle1->longest + 1) % 3], mesh);
    children[1] = get_triangle(triangle1->children[(triangle1->longest + 2) % 3], mesh);

    points[3] = &(triangle2->vertices[(triangle2->longest + 2) % 3]);
    if (point_equals(&(triangle2->vertices[triangle2->longest]), points[0])) {
        points[0] = &(triangle2->vertices[triangle2->longest]);
        children[3] = get_triangle(triangle2->children[(triangle2->longest + 2) % 3], mesh);
        children[2] = get_triangle(triangle2->children[(triangle2->longest + 1) % 3], mesh);
    } else {//TODO: Is it possible?
        points[0] = &(triangle2->vertices[(triangle2->longest + 1) % 3]);
        children[3] = get_triangle(triangle2->children[(triangle2->longest + 1) % 3], mesh);
        children[2] = get_triangle(triangle2->children[(triangle2->longest + 2) % 3], mesh);
    }

    triangles[0] = triangle1;
    triangles[1] = get_new_triangle(mesh);
    init_triangle(triangles[1], points[1]->x, points[1]->y, points[2]->x, points[2]->y, center.x, center.y, mesh->map);
    triangles[2] = triangle2;
    triangles[3] = get_new_triangle(mesh);
    init_triangle(triangles[3], points[3]->x, points[3]->y, points[0]->x, points[0]->y, center.x, center.y, mesh->map);

    //Modify input triangles
    points[2]->x = center.x;
    points[2]->y = center.y;
    points[2]->z = center.z;
//    points[2] = &(triangles[1]->vertices[1]); //useless, but I leave it to indicate that they need to be set before using
    points[0]->x = center.x;
    points[0]->y = center.y;
    points[0]->z = center.z;
//    points[0] = &(triangles[3]->vertices[1]);

    //Set neighbours
    for (int k = 0; k < 4; ++k) {
        fix_longest(triangles[k]);
        triangles[k]->children[triangles[k]->longest] = children[k] != NULL ? children[k]->index : -1;//FIXME: Nie mozna jednak uzyc longest.
        triangles[k]->children[(triangles[k]->longest + 1) % 3] = triangles[(k + 1) % 4]->index;
        triangles[k]->children[(triangles[k]->longest + 2) % 3] = triangles[(k + 3) % 4]->index;
    }

    //Configure neighbours
    for (int i = 0; i < 4; ++i) {
        if (children[i] != NULL) {
            if (children[i]->children[0] == triangles[(i/2)*2]->index) {
                children[i]->children[0] = triangles[i]->index;
            } else if (children[i]->children[1] == triangles[(i/2)*2]->index) {
                children[i]->children[1] = triangles[i]->index;
            } else if (children[i]->children[2] == triangles[(i/2)*2]->index) {
                children[i]->children[2] = triangles[i]->index;
            }
        }


    }

    for (int j = 0; j < 4; ++j) {
        verify_neighbours(triangles[j], mesh);
    }
}
