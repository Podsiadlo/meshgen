#include <stdio.h>

/**
 * AB is the longest, BC is medium, AC is the shortest
 */
struct triangle {
    struct triangle * child;
    struct point * a;
    struct point * b;
    struct point * c;
}

struct point {
    int x;
    int y
}


int main(int argc, char** argv) {
    return 0;
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

struct triangle * split(struct triangle * triangle) {
    struct point * new_point = malloc(); //TODO
    new_point->x = (triangle->a->x + triangle->b->x) / 2; //TODO: check if exists
    new_point->y = (triangle->a->y + triangle->b->y) / 2; //TODO: check if exists 
    
    struct triangle * new_triagle = create_triangle(triangle->a, triangle->c, new_point);
    triangle->a=new_point;
    fix_triangle(triangle);
    //TODO: Co sie stanie z sasiadem albo jak bedzie na brzegu?
    //TODO: Napraw dziecko

    return new_triangle;
}



