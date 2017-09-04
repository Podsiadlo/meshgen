#include <stdio.h>
#include <math.h>
#include "point.c"

struct triangle {
    struct point * a;
    struct point * b;
    struct point * c;
    struct triangle * ab_child;
    struct triangle * ac_child;
    struct triangle * bc_child;
    short longest; //0=ab, 1=bc, 2=ac
};

void refine(struct triangle* parent) {
    if(final(parent)) {
        split(parent);
        restructirize(parent);
        return;
    }
    refine(parent->child);
}

bool final(struct triangle* triangle) {
    return true; //sprawdza czy jest graniczny lub jego child jest ostatni - to drugie ma potencjal na optymalizacje
}


/**
 * Given triangle is modified to represent a "half" of itself.
 * @return pointer to a second "half" triangle.
 */
struct triangle * split(struct triangle * triangle1, struct triangle * triangle2) {
    struct point * center = get_longest_edge_midsection(triangle1);
    
    struct point *a, *b, *c, *d;
    struct triangle *child_ab, *child_bc, *child_cd, *child_ad;
    if (triangle1->longest == 0) {
        a=triangle1->a;
        b=triangle1->c;
        c=triangle1->b;
        child_ab=triangle1->child_ac;
        child_bc=triangle1->child_bc;
    } else if (triangle1->longest == 1) {
        a=triangle1->b;
        b=triangle1->a;
        c=triangle1->c;
        child_ab=triangle1->child_ab;
        child_bc=triangle1->child_ac;
    } else {
        a=triangle1->a;
        b=triangle1->b;
        c=triangle1->c;
        child_ab=triangle1->child_ab;
        child_bc=triangle1->child_bc;
    }

    if (triangle2->longest == 0) {
        d=triangle2->c;
        if (point_equals(triangle2->a, a))//TODO
        child_cd=triangle1->child_ab;
        child_ad=triangle1->child_bc;
    } else if (triangle2->longest == 1) {
        d=triangle2->a;
        child_cd=triangle1->child_ab;//TODO
        child_ad=triangle1->child_bc;
    } else {
        d=triangle2->b;
        child_cd=triangle1->child_ab;//TODO
        child_ad=triangle1->child_bc;
    }
    
    
    
    switch(triangle1->longest) {
    case 0:
        triangle1->child_ab = NULL;//TODO:polowka drugiego
        break;
    case 1:
        break;
    case 2:
        break;
    default:
        //TODO: Error handling
    struct triangle * new_triagle = create_triangle(triangle->a, triangle->c, new_point);
    triangle->a=new_point;
    fix_triangle(triangle);
    fix_triangle(new_triangle);
    //TODO: Co sie stanie z sasiadem albo jak bedzie na brzegu?

    return new_triangle;
}

void fix_triangle(struct triangle * triangle) {
    fix_longest(triangle);
    fix_childs(traingle);
}

void fix_longest(struct triangle * triangle) {
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

void fix_childs(struct triangle * triangle) {
    //TODO Nie wiem jak.
    
}


struct point * get_longest_edge_midsection(const struct triangle * traingle) {
    struct point * a, * b;
    if(triangle->longest == 0) {
        a = triangle->a;
        b = triangle->b;
    } else if(triangle->longest == 0) {
        a = triangle->c;
        b = triangle->b;
    } else if(triangle->longest == 0) {
        a = triangle->a;
        b = triangle->c;
    }
    return get_point((a->x + b->x) / 2, (a->y + b->y) / 2); //TOOD: Check if exists
}

struct point * get_point(int x, int y) {
    return (point*)malloc(sizeof(point));
}



