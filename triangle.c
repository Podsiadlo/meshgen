#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "point.c"

struct triangle {//FIXME
    struct point *a;//deprecated
    struct point *b;
    struct point *c;
    struct point a2;
    struct point b2;
    struct point c2;
    struct triangle *child_ab;
    struct triangle *child_bc;
    struct triangle *child_ac;
    short longest; //0=ab, 1=bc, 2=ac
};

void refine(struct triangle *parent) {
    if (final(parent)) {
        split(parent);
        restructirize(parent);
        return;
    }
    refine(parent->child);
}

bool final(struct triangle *triangle) {
    bool final;
    final = get_next_triangle(get_next_triangle(triangle)) == triangle;
    //TODO Check if it is on border
    return final; //sprawdza czy jest graniczny lub jego child jest ostatni - to drugie ma potencjal na optymalizacje
}

void init_triangle(struct triangle *triangle) {

}
struct triangle *create_triangle(int a_x, int a_y, int b_x, int b_y,  int c_x, int c_y, short** map) {
    struct triangle *triangle = (struct triangle *) malloc(sizeof(struct triangle));
    init_point(&(triangle->a2), a_x, a_y, map[a_x][a_y]);
    init_point(&(triangle->b2), b_x, b_y, map[b_x][b_y]);
    init_point(&(triangle->c2), c_x, c_y, map[c_x][c_y]);
    init_triangle(triangle);
}


/**
 * Given triangle is modified to represent a "half" of itself.
 * @return pointer to a second "half" triangle.
 */
struct triangle *split(struct triangle *triangle1, struct triangle *triangle2) {
    struct point *center = get_longest_edge_midsection(triangle1);

    struct point *a, *b, *c, *d;
    struct triangle *child_ab, *child_bc, *child_cd, *child_ad;

    struct triangle *triangle_abp, *triangle_bcp, *triangle_cdp, *triangle_adp;
    triangle_abp = (struct triangle *) malloc(sizeof(struct triangle));
    triangle_bcp = (struct triangle *) malloc(sizeof(struct triangle));
    triangle_cdp = (struct triangle *) malloc(sizeof(struct triangle));
    triangle_adp = (struct triangle *) malloc(sizeof(struct triangle));

    if (triangle1->longest == 0) {
        a = triangle1->a;
        b = triangle1->c;
        c = triangle1->b;
        child_ab = triangle1->child_ac;
        child_bc = triangle1->child_bc;
    } else if (triangle1->longest == 1) {
        a = triangle1->b;
        b = triangle1->a;
        c = triangle1->c;
        child_ab = triangle1->child_ab;
        child_bc = triangle1->child_ac;
    } else {
        a = triangle1->a;
        b = triangle1->b;
        c = triangle1->c;
        child_ab = triangle1->child_ab;
        child_bc = triangle1->child_bc;
    }

    if (triangle2->longest == 0) {
        d = triangle2->c;
        if (point_equals(triangle2->a, a)) {
            child_ad = triangle2->child_ac;
            child_cd = triangle2->child_bc;
        } else {
            child_ad = triangle2->child_bc;
            child_cd = triangle2->child_ac;
        }
    } else if (triangle2->longest == 1) {
        d = triangle2->a;
        if (point_equals(triangle2->b, a)) {
            child_ad = triangle2->child_ab;
            child_cd = triangle2->child_ac;
        } else {
            child_ad = triangle2->child_ac;
            child_cd = triangle2->child_ab;
        }
    } else if (triangle2->longest == 2) {
        d = triangle2->b;
        if (point_equals(triangle2->a, a)) {
            child_ad = triangle2->child_ab;
            child_cd = triangle2->child_bc;
        } else {
            child_ad = triangle2->child_bc;
            child_cd = triangle2->child_ab;
        }
    } else {
        //TODO error handling
    }

    if (child_ab->child_ab == triangle1) {
        child_ab->child_ab = triangle_abp;
    } else if (child_ab->child_bc == triangle1) {
        child_ab->child_bc = triangle_abp;
    } else if (child_ab->child_ac == triangle1) {
        child_ab->child_ac = triangle_abp;
    }

    if (child_bc->child_ab == triangle1) {
        child_bc->child_ab = triangle_bcp;
    } else if (child_bc->child_bc == triangle1) {
        child_bc->child_bc = triangle_bcp;
    } else if (child_bc->child_ac == triangle1) {
        child_bc->child_ac = triangle_bcp;
    }

    if (child_cd->child_ab == triangle2) {
        child_cd->child_ab = triangle_cdp;
    } else if (child_cd->child_bc == triangle2) {
        child_cd->child_bc = triangle_cdp;
    } else if (child_cd->child_ac == triangle2) {
        child_cd->child_ac = triangle_cdp;
    }

    if (child_ad->child_ab == triangle2) {
        child_ad->child_ab = triangle_adp;
    } else if (child_ad->child_bc == triangle2) {
        child_ad->child_bc = triangle_adp;
    } else if (child_ad->child_ac == triangle2) {
        child_ad->child_ac = triangle_adp;
    }

    triangle_abp->a = a;
    triangle_abp->b = b;
    triangle_abp->c = center;
    triangle_abp->child_ab = child_ab;
    triangle_abp->child_bc = triangle_bcp;
    triangle_abp->child_ac = triangle_adp;

    triangle_bcp->a = b;
    triangle_bcp->b = c;
    triangle_bcp->c = center;
    triangle_bcp->child_ab = child_bc;
    triangle_bcp->child_bc = triangle_cdp;
    triangle_bcp->child_ac = triangle_abp;

    triangle_cdp->a = c;
    triangle_cdp->b = d;
    triangle_cdp->c = center;
    triangle_cdp->child_ab = child_cd;
    triangle_cdp->child_bc = triangle_adp;
    triangle_cdp->child_ac = triangle_bcp;

    triangle_adp->a = d;
    triangle_adp->b = a;
    triangle_adp->c = center;
    triangle_adp->child_ab = child_ad;
    triangle_adp->child_bc = triangle_cdp;
    triangle_adp->child_ac = triangle_abp;
    //child_ad->

    fix_triangle(triangle_abp);
    fix_triangle(triangle_bcp);
    fix_triangle(triangle_cdp);
    fix_triangle(triangle_adp);
    //TODO: Co sie stanie z sasiadem albo jak bedzie na brzegu?

    return new_triangle;
}

void fix_triangle(struct triangle *triangle) {
    fix_longest(triangle);
    // fix_childs(triangle); //Zakomentowane, bo po miesiacu nie pamietam, czy tego nie chciałem wywalić…
}

void fix_longest(struct triangle *triangle) {
    double ab = sqrt(pow(triangle->a->x - triangle->b->x, 2) + pow(triangle->a->y - triangle->b->y, 2));
    double bc = sqrt(pow(triangle->b->x - triangle->c->x, 2) + pow(triangle->b->y - triangle->c->y, 2));
    double ac = sqrt(pow(triangle->a->x - triangle->c->x, 2) + pow(triangle->a->y - triangle->c->y, 2));

    int longest;
    if (ab > bc) {
        if (ab > ac) {
            longest = 0;
        } else {
            longest = 2;
        }
    } else {
        if (bc > ac) {
            longest = 1;
        } else {
            longest = 2;
        }
    }
    triangle->longest = longest;
}

void fix_childs(struct triangle *triangle) {
    //TODO Nie wiem jak.

}

struct triangle *get_next_triangle(struct triangle *triangle) {
    if (triangle->longest == 0) {
        return triangle->child_ab;
    } else if (triangle->longest == 1) {
        return triangle->child_bc;
    } else if (triangle->longest == 2) {
        return triangle->child_ac;
    }
    //TODO Error
}


struct point *get_longest_edge_midsection(const struct triangle *triangle) {
    struct point *a, *b;
    if (triangle->longest == 0) {
        a = triangle->a;
        b = triangle->b;
    } else if (triangle->longest == 0) {
        a = triangle->c;
        b = triangle->b;
    } else if (triangle->longest == 0) {
        a = triangle->a;
        b = triangle->c;
    }
    return create_point((a->x + b->x) / 2, (a->y + b->y) / 2); //TOOD: Check if exists
}
