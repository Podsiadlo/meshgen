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
    if (fabs(triangle->vertices[triangle->longest].x - triangle->vertices[(1 + triangle->longest) % 3].x) < 1
        && fabs(triangle->vertices[triangle->longest].y - triangle->vertices[(1 + triangle->longest) % 3].y) < 1) {

        return true;
    }
    return false;
}

bool inside_condition(const struct triangle *triangle, struct mesh *mesh) {
    return fabs(get_height_mean(triangle) - get_height_of_center(triangle, mesh->map)) > EPSILON;
}

bool outside_condition(struct triangle *triangle, struct mesh *mesh) {
    int center = get_height_of_center(triangle, mesh->map);
    double ab, bc, ac;
    if (triangle->children[0] != -1) {
        ab = fabs(fabs(triangle->vertices[0].z - triangle->vertices[1].z) -
                  fabs(get_height_of_center(get_triangle(triangle->children[0], mesh), mesh->map) - center));
        if (ab > EPSILON) {
            return true;
        }
    }
    if (triangle->children[1] != -1) {
        bc = fabs(fabs(triangle->vertices[2].z - triangle->vertices[1].z) -
                  fabs(get_height_of_center(get_triangle(triangle->children[1], mesh), mesh->map) - center));
        if (bc > EPSILON) {
            return true;
        }
    }
    if (triangle->children[2] != -1) {
        ac = fabs(fabs(triangle->vertices[0].z - triangle->vertices[2].z) -
                  fabs(get_height_of_center(get_triangle(triangle->children[2], mesh), mesh->map) - center));
        if (ac > EPSILON) {
            return true;
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
    init_triangle(new_triangle, triangle->vertices[longest].x, triangle->vertices[longest].y,
                  triangle->vertices[(longest + 2) % 3].x, triangle->vertices[(longest + 2) % 3].y,
                  center.x, center.y, mesh->map);
    new_triangle->children[0] = triangle->children[(longest + 2) % 3];
    new_triangle->children[1] = triangle->index;
    new_triangle->children[2] = -1;

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
}


void split_inner(struct triangle *triangle1, struct triangle *triangle2, struct mesh *mesh) {
    struct point center;
    get_longest_edge_midsection(&center, triangle1);
    center.z = mesh->map[center.y][center.x];

    struct point *a, *b, *c, *d;
    struct triangle *child_ab, *child_bc, *child_cd, *child_ad;
    struct triangle *triangle_abp, *triangle_bcp, *triangle_cdp, *triangle_adp;

    //Simplify symbols
    a = &(triangle1->vertices[triangle1->longest]);
    b = &(triangle1->vertices[(triangle1->longest + 2) % 3]);
    c = &(triangle1->vertices[(triangle1->longest + 1) % 3]);
    child_ab = get_triangle(triangle1->children[(triangle1->longest + 2) % 3], mesh);
    child_bc = get_triangle(triangle1->children[(triangle1->longest + 1) % 3], mesh);

    d = &(triangle2->vertices[(triangle2->longest + 2) % 3]);
    if (point_equals(&(triangle2->vertices[triangle2->longest]), a)) {
        a = &(triangle2->vertices[triangle2->longest]);
        child_ad = get_triangle(triangle2->children[(triangle2->longest + 2) % 3], mesh);
        child_cd = get_triangle(triangle2->children[(triangle2->longest + 1) % 3], mesh);
    } else {
        a = &(triangle2->vertices[(triangle2->longest + 1) % 3]);
        child_ad = get_triangle(triangle2->children[(triangle2->longest + 1) % 3], mesh);
        child_cd = get_triangle(triangle2->children[(triangle2->longest + 2) % 3], mesh);
    }

    triangle_abp = triangle1;
    triangle_bcp = get_new_triangle(mesh);
    init_triangle(triangle_bcp, b->x, b->y, c->x, c->y, center.x, center.y, mesh->map);
    triangle_cdp = triangle2;
    triangle_adp = get_new_triangle(mesh);
    init_triangle(triangle_adp, d->x, d->y, a->x, a->y, center.x, center.y, mesh->map);

    //Modify input triangles
    c->x = center.x;
    c->y = center.y;
    c->z = center.z;
//    c = &(triangle_bcp->vertices[1]); //useless, but I leave it to indicate that they need to be set before using
    a->x = center.x;
    a->y = center.y;
    a->z = center.z;
//    a = &(triangle_adp->vertices[1]);

    //Set neighbours
    triangle_abp->children[0] = child_ab != NULL ? child_ab->index : -1;
    triangle_abp->children[1] = triangle_bcp->index;
    triangle_abp->children[2] = triangle_adp->index;

    triangle_bcp->children[0] = child_bc != NULL ? child_bc->index : -1;
    triangle_bcp->children[1] = triangle_cdp->index;
    triangle_bcp->children[2] = triangle_abp->index;

    triangle_cdp->children[0] = child_cd != NULL ? child_cd->index : -1;
    triangle_cdp->children[1] = triangle_adp->index;
    triangle_cdp->children[2] = triangle_bcp->index;

    triangle_adp->children[0] = child_ad != NULL ? child_ad->index : -1;
    triangle_adp->children[1] = triangle_abp->index;
    triangle_adp->children[2] = triangle_cdp->index;

    //Configure neighbours
    if (child_ab != NULL) {
        if (child_ab->children[0] == triangle1->index) {
            child_ab->children[0] = triangle_abp->index;
        } else if (child_ab->children[1] == triangle1->index) {
            child_ab->children[1] = triangle_abp->index;
        } else if (child_ab->children[2] == triangle1->index) {
            child_ab->children[2] = triangle_abp->index;
        }
    }

    if (child_bc != NULL) {
        if (child_bc->children[0] == triangle1->index) {
            child_bc->children[0] = triangle_bcp->index;
        } else if (child_bc->children[1] == triangle1->index) {
            child_bc->children[1] = triangle_bcp->index;
        } else if (child_bc->children[2] == triangle1->index) {
            child_bc->children[2] = triangle_bcp->index;
        }
    }

    if (child_cd != NULL) {
        if (child_cd->children[0] == triangle2->index) {
            child_cd->children[0] = triangle_cdp->index;
        } else if (child_cd->children[1] == triangle2->index) {
            child_cd->children[1] = triangle_cdp->index;
        } else if (child_cd->children[2] == triangle2->index) {
            child_cd->children[2] = triangle_cdp->index;
        }
    }

    if (child_ad != NULL) {
        if (child_ad->children[0] == triangle2->index) {
            child_ad->children[0] = triangle_adp->index;
        } else if (child_ad->children[1] == triangle2->index) {
            child_ad->children[1] = triangle_adp->index;
        } else if (child_ad->children[2] == triangle2->index) {
            child_ad->children[2] = triangle_adp->index;
        }
    }

    fix_longest(triangle_abp);
    fix_longest(triangle_bcp);
    fix_longest(triangle_cdp);
    fix_longest(triangle_adp);
}
