#include <stdio.h>

struct point {
    int x;
    int y
};

struct point * get_point(int x, int y) {
    return (point*)malloc(sizeof(point));
}

