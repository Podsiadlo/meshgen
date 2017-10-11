#include <math.h>
#include <stdlib.h>
#include "refinement.h"


void refine(struct triangle *triangle, struct mesh *mesh) {
    if (is_final_step(triangle)) {
        split(triangle, mesh);
    } else {
        refine(get_next_triangle(triangle), mesh);
    }
}


bool refine_if_required(struct triangle *triangle, struct mesh *mesh) {
    bool refined = false;
    if (inside_condition(triangle, mesh->map) || outside_condition(triangle, mesh->map)) {
        refine(triangle, mesh);
        refined = true;
    }
    return refined;
}

bool inside_condition(const struct triangle *triangle, const short **map) {
    return fabs(get_height_mean(triangle) - get_height_of_center(triangle, map)) > EPSILON;
}

bool outside_condition(struct triangle *triangle, const short **map) { //todo optimize
    int center = get_height_of_center(triangle, map);
    double ab, bc, ac;
    if (triangle->child_ab != NULL) {
        ab = fabs(fabs(triangle->a2.z - triangle->b2.z) - fabs(get_height_of_center(triangle->child_ab, map) - center));
    } else {
        ab = 0;
    }
    if (triangle->child_bc != NULL) {
        bc = fabs(fabs(triangle->c2.z - triangle->b2.z) - fabs(get_height_of_center(triangle->child_bc, map) - center));
    } else {
        bc = 0;
    }
    if (triangle->child_ac != NULL) {
        ac = fabs(fabs(triangle->a2.z - triangle->c2.z) - fabs(get_height_of_center(triangle->child_ac, map) - center));
    } else {
        ac = 0;
    }
    return ab > EPSILON || bc > EPSILON || ac > EPSILON;
}

bool is_final_step(struct triangle *triangle) {
    struct triangle *next_triangle = get_next_triangle(triangle);
    return next_triangle == NULL || get_next_triangle(next_triangle) == triangle;
}

void split(struct triangle *triangle, struct mesh *mesh) {
    struct triangle *next_triangle = get_next_triangle(triangle);
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
    if (triangle->longest == 0) {
        new_triangle = create_triangle(triangle->a2.x, triangle->a2.y, triangle->c2.x, triangle->c2.y,
                                       center.x, center.y, mesh);
        triangle->a2.x = center.x;
        triangle->a2.y = center.y;
        triangle->a2.z = mesh->map[triangle->a2.y][triangle->a2.x];
        new_triangle->child_ab = triangle->child_ac;
        new_triangle->child_bc = triangle;
        new_triangle->child_ac = NULL;
        triangle->child_ac = new_triangle;
    } else if (triangle->longest == 1) {
        new_triangle = create_triangle(triangle->a2.x, triangle->a2.y, triangle->b2.x, triangle->b2.y,
                                       center.x, center.y, mesh);
        triangle->b2.x = center.x;
        triangle->b2.y = center.y;
        triangle->b2.z = mesh->map[triangle->b2.y][triangle->b2.x];
        new_triangle->child_ab = triangle->child_ab;
        new_triangle->child_bc = NULL;
        new_triangle->child_ac = triangle;
        triangle->child_ab = new_triangle;
    } else {
        new_triangle = create_triangle(triangle->b2.x, triangle->b2.y, triangle->c2.x, triangle->c2.y,
                                       center.x, center.y, mesh);
        triangle->c2.x = center.x;
        triangle->c2.y = center.y;
        triangle->c2.z = mesh->map[triangle->c2.y][triangle->c2.x];
        new_triangle->child_ab = triangle->child_bc;
        new_triangle->child_bc = NULL;
        new_triangle->child_ac = triangle;
        triangle->child_bc = new_triangle;
    }
    fix_longest(triangle);
    fix_longest(new_triangle);
}


/**
 * Given triangle is modified to represent a "half" of itself.
 * @return pointer to a second "half" triangle.
 */
void split_inner(struct triangle *triangle1, struct triangle *triangle2, struct mesh *mesh) {
    struct point center;
    get_longest_edge_midsection(&center, triangle1);
    center.z = mesh->map[center.y][center.x];

    struct point *a, *b, *c, *d;
    struct triangle *child_ab, *child_bc, *child_cd, *child_ad;
    struct triangle *triangle_abp, *triangle_bcp, *triangle_cdp, *triangle_adp;

    //unifying symbols
    if (triangle1->longest == 0) {
        a = &(triangle1->a2);
        b = &(triangle1->c2);
        c = &(triangle1->b2);
        child_ab = triangle1->child_ac;
        child_bc = triangle1->child_bc;
    } else if (triangle1->longest == 1) {
        a = &(triangle1->b2);
        b = &(triangle1->a2);
        c = &(triangle1->c2);
        child_ab = triangle1->child_ab;
        child_bc = triangle1->child_ac;
    } else {
        a = &(triangle1->a2);
        b = &(triangle1->b2);
        c = &(triangle1->c2);
        child_ab = triangle1->child_ab;
        child_bc = triangle1->child_bc;
    }

    if (triangle2->longest == 0) {
        d = &(triangle2->c2);
        if (point_equals(&(triangle2->a2), a)) {
            a = &(triangle2->a2);
            child_ad = triangle2->child_ac;
            child_cd = triangle2->child_bc;
        } else {
            a = &(triangle2->b2);
            child_ad = triangle2->child_bc;
            child_cd = triangle2->child_ac;
        }
    } else if (triangle2->longest == 1) {
        d = &(triangle2->a2);
        if (point_equals(&(triangle2->b2), a)) {
            a = &(triangle2->b2);
            child_ad = triangle2->child_ab;
            child_cd = triangle2->child_ac;
        } else {
            a = &(triangle2->c2);
            child_ad = triangle2->child_ac;
            child_cd = triangle2->child_ab;
        }
    } else {
        d = &(triangle2->b2);
        if (point_equals(&(triangle2->a2), a)) {
            a = &(triangle2->a2);
            child_ad = triangle2->child_ab;
            child_cd = triangle2->child_bc;
        } else {
            a = &(triangle2->c2);
            child_ad = triangle2->child_bc;
            child_cd = triangle2->child_ab;
        }
    }

    triangle_abp = triangle1;
    triangle_bcp = create_triangle(b->x, b->y, c->x, c->y, center.x, center.y, mesh);
    triangle_cdp = triangle2;
    triangle_adp = create_triangle(d->x, d->y, a->x, a->y, center.x, center.y, mesh);

    //modifying input triangles
    c->x = center.x;
    c->y = center.y;
    c->z = center.z;
//    c = &(triangle_bcp->b2); //useless, but I leave it to indicate that they need to be set before using
    a->x = center.x;
    a->y = center.y;
    a->z = center.z;
//    a = &(triangle_adp->b2);

    //setting neighbours
    triangle_abp->child_ab = child_ab;
    triangle_abp->child_bc = triangle_bcp;
    triangle_abp->child_ac = triangle_adp;

    triangle_bcp->child_ab = child_bc;
    triangle_bcp->child_bc = triangle_cdp;
    triangle_bcp->child_ac = triangle_abp;

    triangle_cdp->child_ab = child_cd;
    triangle_cdp->child_bc = triangle_adp;
    triangle_cdp->child_ac = triangle_bcp;

    triangle_adp->child_ab = child_ad;
    triangle_adp->child_bc = triangle_cdp;
    triangle_adp->child_ac = triangle_abp;

    //configuring neighbours
    if (child_ab != NULL) {
        if (child_ab->child_ab == triangle1) {
            child_ab->child_ab = triangle_abp;
        } else if (child_ab->child_bc == triangle1) {
            child_ab->child_bc = triangle_abp;
        } else if (child_ab->child_ac == triangle1) {
            child_ab->child_ac = triangle_abp;
        }
    }

    if (child_ab != NULL) {
        if (child_bc->child_ab == triangle1) {
            child_bc->child_ab = triangle_bcp;
        } else if (child_bc->child_bc == triangle1) {
            child_bc->child_bc = triangle_bcp;
        } else if (child_bc->child_ac == triangle1) {
            child_bc->child_ac = triangle_bcp;
        }
    }

    if (child_ab != NULL) {
        if (child_cd->child_ab == triangle2) {
            child_cd->child_ab = triangle_cdp;
        } else if (child_cd->child_bc == triangle2) {
            child_cd->child_bc = triangle_cdp;
        } else if (child_cd->child_ac == triangle2) {
            child_cd->child_ac = triangle_cdp;
        }
    }

    if (child_ab != NULL) {
        if (child_ad->child_ab == triangle2) {
            child_ad->child_ab = triangle_adp;
        } else if (child_ad->child_bc == triangle2) {
            child_ad->child_bc = triangle_adp;
        } else if (child_ad->child_ac == triangle2) {
            child_ad->child_ac = triangle_adp;
        }
    }

    fix_longest(triangle_abp);
    fix_longest(triangle_bcp);
    fix_longest(triangle_cdp);
    fix_longest(triangle_adp);
}
