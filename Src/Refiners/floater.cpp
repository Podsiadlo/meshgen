#include "floater.h"
#include "../DataStructures/point.h"
#include <stdlib.h>
#include <vector>
#include <cmath>

struct Face{
    int size;
    struct point **nodes;
    int nodes_size;
};

void floater(struct Face * faces, int faces_size) {


    for (int i = 0; i < faces_size; ++i) {
        for (int j = 0; j < faces[i].nodes_size; ++j) {
            compute_normal(faces[i].nodes[j]);
        }
    }

    for (int i = 0; i < faces_size; ++i) {
        for (int j = 0; j < faces[i].nodes_size; ++j) {
            std::vector<double> normal = faces[i].nodes[j]->normal;
            struct point * next_node = faces[i].nodes[(j+1)%faces[i].nodes_size];
            struct point * previous_node = faces[i].nodes[(j-1)%faces[i].nodes_size];
            std::vector<double> p, next_p, prev_p;

            p = scalar_product(faces[i].nodes[j], normal);
            next_p;
            prev_p;

            double cos_angle = product(next_p, prev_p) / (length(next_p) * length(prev_p));
            double angle = acos(cos_angle);
            double tg = tan(angle/2.);
            double val1 = tg / length(next_p);
            double val2 = tg / length(prev_p);
        }
    }
}

void compute_normal(struct point * node) {

}

std::vector<double> scalar_product(struct point *point, std::vector<double> &vector) {

}

double product(std::vector v1, std::vector v2) {

}

std::vector<double> substract(std::vector<double> &a, std::vector<double> &b) {

}

double length(std::vector<double> &v) {

}