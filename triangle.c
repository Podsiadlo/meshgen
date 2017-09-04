#include <stdio.h>

struct triangle {
    struct point * a;
    struct point * b;
    struct point * c;
    struct triangle * ab_child;
    struct triangle * ac_child;
    struct triangle * bc_child;
    int longest; //0=ab, 1=bc, 2=ac
}

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
struct triangle * split(struct triangle * triangle) {
    struct point * midsection = get_longest_edge_midsection(triangle);
    
    struct triangle * new_triagle = create_triangle(triangle->a, triangle->c, new_point);
    triangle->a=new_point;
    fix_triangle(triangle);
    fix_triangle(new_triangle);
    //TODO: Co sie stanie z sasiadem albo jak bedzie na brzegu?

    return new_triangle;
}

void fix_triangle(struct triangle * triangle) {
    
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



