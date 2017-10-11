#include <math.h>
#include <stdlib.h>
#include "refinement.h"


bool refine_if_required(struct triangle *triangle, struct mesh *mesh) {
    bool refined = false;
    if (inside_condition(triangle, mesh) || outside_condition(triangle, mesh)) {
        refine(triangle, mesh);
        refined = true;
    }
    return refined;
}


bool inside_condition(const struct triangle *triangle, struct mesh *mesh) {
    return fabs(get_height_mean(triangle) - get_height_of_center(triangle, mesh->map)) > EPSILON;
}

bool outside_condition(struct triangle *triangle, struct mesh *mesh) { //todo optimize
    int center = get_height_of_center(triangle, mesh->map);
    double ab, bc, ac;
    if (triangle->child_ab != -1) {
        ab = fabs(fabs(triangle->a.z - triangle->b.z) - fabs(get_height_of_center(mesh->triangles[triangle->child_ab], mesh->map) - center));
    } else {
        ab = 0;
    }
    if (triangle->child_bc != -1) {
        bc = fabs(fabs(triangle->c.z - triangle->b.z) - fabs(get_height_of_center(mesh->triangles[triangle->child_bc], mesh->map) - center));
    } else {
        bc = 0;
    }
    if (triangle->child_ac != -1) {
        ac = fabs(fabs(triangle->a.z - triangle->c.z) - fabs(get_height_of_center(mesh->triangles[triangle->child_ac], mesh->map) - center));
    } else {
        ac = 0;
    }
    return ab > EPSILON || bc > EPSILON || ac > EPSILON;
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
    if (triangle->longest == 0) {
        new_triangle = create_triangle(triangle->a.x, triangle->a.y, triangle->c.x, triangle->c.y,
                                       center.x, center.y, mesh->map);
        add_triangle(new_triangle, mesh);
        triangle->a.x = center.x;
        triangle->a.y = center.y;
        triangle->a.z = mesh->map[triangle->a.y][triangle->a.x];
        new_triangle->child_ab = triangle->child_ac;
        new_triangle->child_bc = triangle->index;
        new_triangle->child_ac = -1;
        triangle->child_ac = new_triangle->index;
    } else if (triangle->longest == 1) {
        new_triangle = create_triangle(triangle->a.x, triangle->a.y, triangle->b.x, triangle->b.y,
                                       center.x, center.y, mesh->map);
        add_triangle(new_triangle, mesh);
        triangle->b.x = center.x;
        triangle->b.y = center.y;
        triangle->b.z = mesh->map[triangle->b.y][triangle->b.x];
        new_triangle->child_ab = triangle->child_ab;
        new_triangle->child_bc = -1;
        new_triangle->child_ac = triangle->index;
        triangle->child_ab = new_triangle->index;
    } else {
        new_triangle = create_triangle(triangle->b.x, triangle->b.y, triangle->c.x, triangle->c.y,
                                       center.x, center.y, mesh->map);
        add_triangle(new_triangle, mesh);
        triangle->c.x = center.x;
        triangle->c.y = center.y;
        triangle->c.z = mesh->map[triangle->c.y][triangle->c.x];
        new_triangle->child_ab = triangle->child_bc;
        new_triangle->child_bc = -1;
        new_triangle->child_ac = triangle->index;
        triangle->child_bc = new_triangle->index;
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

    //unifying symbols
    if (triangle1->longest == 0) {
        a = &(triangle1->a);
        b = &(triangle1->c);
        c = &(triangle1->b);
        child_ab = get_triangle(triangle1->child_ac, mesh);
        child_bc = get_triangle(triangle1->child_bc, mesh);
    } else if (triangle1->longest == 1) {
        a = &(triangle1->b);
        b = &(triangle1->a);
        c = &(triangle1->c);
        child_ab = get_triangle(triangle1->child_ab, mesh);
        child_bc = get_triangle(triangle1->child_ac, mesh);
    } else {
        a = &(triangle1->a);
        b = &(triangle1->b);
        c = &(triangle1->c);
        child_ab = get_triangle(triangle1->child_ab, mesh);
        child_bc = get_triangle(triangle1->child_bc, mesh);
    }

    if (triangle2->longest == 0) {
        d = &(triangle2->c);
        if (point_equals(&(triangle2->a), a)) {
            a = &(triangle2->a);
            child_ad = get_triangle(triangle2->child_ac, mesh);
            child_cd = get_triangle(triangle2->child_bc, mesh);
        } else {
            a = &(triangle2->b);
            child_ad = get_triangle(triangle2->child_bc, mesh);
            child_cd = get_triangle(triangle2->child_ac, mesh);
        }
    } else if (triangle2->longest == 1) {
        d = &(triangle2->a);
        if (point_equals(&(triangle2->b), a)) {
            a = &(triangle2->b);
            child_ad = get_triangle(triangle2->child_ab, mesh);
            child_cd = get_triangle(triangle2->child_ac, mesh);
        } else {
            a = &(triangle2->c);
            child_ad = get_triangle(triangle2->child_ac, mesh);
            child_cd = get_triangle(triangle2->child_ab, mesh);
        }
    } else {
        d = &(triangle2->b);
        if (point_equals(&(triangle2->a), a)) {
            a = &(triangle2->a);
            child_ad = get_triangle(triangle2->child_ab, mesh);
            child_cd = get_triangle(triangle2->child_bc, mesh);
        } else {
            a = &(triangle2->c);
            child_ad = get_triangle(triangle2->child_bc, mesh);
            child_cd = get_triangle(triangle2->child_ab, mesh);
        }
    }

    triangle_abp = triangle1;
    triangle_bcp = create_triangle(b->x, b->y, c->x, c->y, center.x, center.y, mesh->map);
        triangle_bcp->index = add_triangle(triangle_bcp, mesh);
    triangle_cdp = triangle2;
    triangle_adp = create_triangle(d->x, d->y, a->x, a->y, center.x, center.y, mesh->map);
        triangle_adp->index = add_triangle(triangle_adp, mesh);

    //modifying input triangles
    c->x = center.x;
    c->y = center.y;
    c->z = center.z;
//    c = &(triangle_bcp->b); //useless, but I leave it to indicate that they need to be set before using
    a->x = center.x;
    a->y = center.y;
    a->z = center.z;
//    a = &(triangle_adp->b);

    //setting neighbours
    triangle_abp->child_ab = child_ab != NULL ? child_ab->index : -1;
    triangle_abp->child_bc = triangle_bcp->index;
    triangle_abp->child_ac = triangle_adp->index;

    triangle_bcp->child_ab = child_bc != NULL ? child_bc->index : -1;
    triangle_bcp->child_bc = triangle_cdp->index;
    triangle_bcp->child_ac = triangle_abp->index;

    triangle_cdp->child_ab = child_cd != NULL ? child_cd->index : -1;
    triangle_cdp->child_bc = triangle_adp->index;
    triangle_cdp->child_ac = triangle_bcp->index;

    triangle_adp->child_ab = child_ad != NULL ? child_ad->index : -1;
    triangle_adp->child_bc = triangle_cdp->index;
    triangle_adp->child_ac = triangle_abp->index;

    //configuring neighbours
    if (child_ab != NULL) {
        if (child_ab->child_ab == triangle1->index) {
            child_ab->child_ab = triangle_abp->index;
        } else if (child_ab->child_bc == triangle1->index) {
            child_ab->child_bc = triangle_abp->index;
        } else if (child_ab->child_ac == triangle1->index) {
            child_ab->child_ac = triangle_abp->index;
        }
    }

    if (child_ab != NULL) {
        if (child_bc->child_ab == triangle1->index) {
            child_bc->child_ab = triangle_bcp->index;
        } else if (child_bc->child_bc == triangle1->index) {
            child_bc->child_bc = triangle_bcp->index;
        } else if (child_bc->child_ac == triangle1->index) {
            child_bc->child_ac = triangle_bcp->index;
        }
    }

    if (child_ab != NULL) {
        if (child_cd->child_ab == triangle2->index) {
            child_cd->child_ab = triangle_cdp->index;
        } else if (child_cd->child_bc == triangle2->index) {
            child_cd->child_bc = triangle_cdp->index;
        } else if (child_cd->child_ac == triangle2->index) {
            child_cd->child_ac = triangle_cdp->index;
        }
    }

    if (child_ab != NULL) {
        if (child_ad->child_ab == triangle2->index) {
            child_ad->child_ab = triangle_adp->index;
        } else if (child_ad->child_bc == triangle2->index) {
            child_ad->child_bc = triangle_adp->index;
        } else if (child_ad->child_ac == triangle2->index) {
            child_ad->child_ac = triangle_adp->index;
        }
    }

    fix_longest(triangle_abp);
    fix_longest(triangle_bcp);
    fix_longest(triangle_cdp);
    fix_longest(triangle_adp);
}
