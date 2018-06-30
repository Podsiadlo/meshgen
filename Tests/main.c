#include <check.h>

#include "mesh.h"
#include "refinement.h"
#include "output.h"
#include "utils.h"

#include <stdint.h>
#include <stdlib.h>

Suite *
meshgen_suite(void);

START_TEST(test_refine_new_mesh)
{

    // Simple 5x5 map with all heights zero except for one with 100m
    double **map_data = init_map_data(5, 5);
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            map_data[i][j] = 0;
        }
    }
    map_data[2][3] = 100;

    // Generate the initial mesh
    struct map * map = init_map((const double **) map_data, 5, 5, 1, 1);
    map->north_border = 4;
    map->west_border = 0;
    struct mesh *local_mesh = generate_mesh(map, 5, 0);

    refine_new_mesh(local_mesh, 99, false);

    ck_assert_int_eq(local_mesh->counter, 13);

    free_mesh(local_mesh);
}
END_TEST

START_TEST(test_refine)
{

    // Simple 5x5 map with all heights zero except for one with 100m
    double **map_data = init_map_data(5, 5);
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            map_data[i][j] = 0;
        }
    }
    map_data[2][3] = 100;

    // Generate the initial mesh
    struct map * map = init_map((const double **) map_data, 5, 5, 1, 1);
    map->north_border = 4;
    map->west_border = 0;
    struct mesh *local_mesh = generate_mesh(map, 5, 0);

    struct triangle *triangle_to_refine = &(local_mesh->triangles[0]);

    refine(triangle_to_refine, local_mesh, NULL);

    ck_assert_int_eq(local_mesh->counter, 4);

    refine(triangle_to_refine, local_mesh, NULL);

    ck_assert_int_eq(local_mesh->counter, 5);

    refine(triangle_to_refine, local_mesh, NULL);

    ck_assert_int_eq(local_mesh->counter, 8);

    refine(triangle_to_refine, local_mesh, NULL);

    ck_assert_int_eq(local_mesh->counter, 13);

    free_mesh(local_mesh);
}
END_TEST

START_TEST(test_is_inside_triangle)
    {

        double step = 1;
        int lower_boundary = 1;
        double upper_boundary = 6.5;
        double coords[3];
        struct point *point;
        struct triangle * triangle = (struct triangle *)malloc(sizeof(struct triangle));

        triangle->vertices[0].x = lower_boundary;
        triangle->vertices[0].y = lower_boundary;
        triangle->vertices[1].x = upper_boundary;
        triangle->vertices[1].y = upper_boundary;
        triangle->vertices[2].x = upper_boundary;
        triangle->vertices[2].y = lower_boundary;

        point = malloc(sizeof(struct point));
        point->x = 0;
        point->y = 2;
        compute_barycentric_coords(coords, point,triangle);
        free(point);
        ck_assert(!is_inside_triangle(coords));

        point = malloc(sizeof(struct point));
        point->x = 2;
        point->y = 0;
        compute_barycentric_coords(coords, point,triangle);
        free(point);
        ck_assert(!is_inside_triangle(coords));

        point = malloc(sizeof(struct point));
        point->x = 7;
        point->y = 3;
        compute_barycentric_coords(coords, point,triangle);
        free(point);
        ck_assert(!is_inside_triangle(coords));

        point = malloc(sizeof(struct point));
        point->x = 6;
        point->y = 3;
        compute_barycentric_coords(coords, point,triangle);
        free(point);
        ck_assert(is_inside_triangle(coords));

        point = malloc(sizeof(struct point));
        point->x = 6.5;
        point->y = 5;
        compute_barycentric_coords(coords, point,triangle);
        free(point);
        ck_assert(is_inside_triangle(coords));

        point = malloc(sizeof(struct point));
        point->x = 6.5;
        point->y = 6.5;
        compute_barycentric_coords(coords, point,triangle);
        free(point);
        ck_assert(is_inside_triangle(coords));

        int points_inside = 0;
        for (double i = 0; i <= floor(upper_boundary); i+= step) {
            for (double j = 0; j <= floor(upper_boundary); j+= step) {
                point = malloc(sizeof(struct point));
                point->x = i;
                point->y = j;
                compute_barycentric_coords(coords, point,triangle);
                points_inside += is_inside_triangle(coords) ? 1 : 0;
                free(point);
            }
        }

        double n = floor((upper_boundary) / step);
        double expected = (lower_boundary + (n - 1) / 2) * n;

        ck_assert_int_eq(points_inside, expected);

        free(triangle);
    }
END_TEST


START_TEST(test_get_height)
{
    double **map_data = init_map_data(5, 5);
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            map_data[i][j] = (j / 2) * 10 * ((i / 2) + 1);
        }
    }
    struct map * map = init_map((const double **) map_data, 5, 5, 1, 1);
    map->north_border = 4;
    map->west_border = 0;
    //0 0 10 10 20
    //0 0 10 10 20
    //0 0 20 20 40
    //0 0 20 20 40
    //0 0 30 30 60
    ck_assert_double_eq_tol(get_height(0, 0, map), 0, 0.00001);
    ck_assert_double_eq_tol(get_height(4, 4, map), 20, 0.00001);
    ck_assert_double_eq_tol(get_height(1, 0.5, map), 0, 0.00001);
    ck_assert_double_eq_tol(get_height(0.5, 4, map), 0, 0.00001);
    ck_assert_double_eq_tol(get_height(0.5, 3.5, map), 0, 0.00001);
    ck_assert_double_eq_tol(get_height(2, 2, map), 20, 0.00001);
    ck_assert_double_eq_tol(get_height(2.25, 2.75, map), 12.5, 0.00001);
    ck_assert_double_eq_tol(get_height(3.5, 0, map), 45, 0.00001);
    ck_assert_double_eq_tol(get_height(3.5, 0.5, map), 37.5, 0.00001);
    ck_assert_double_eq_tol(get_height(2.5, 1.5, map), 20, 0.00001);
    ck_assert_double_eq_tol(get_height(3.5, 1.5, map), 30, 0.00001);
    ck_assert_double_eq_tol(get_height(3.25, 3.25, map), 12.5, 0.00001);

    free_map(map);
}
END_TEST

START_TEST(test_change_bytes_order)
    {
        uint16_t a = 1;
        uint16_t a_expected = 256;
        uint16_t b = 65535;
        uint16_t b_expected = 65535;
        uint16_t c = 512;
        uint16_t c_expected = 2;

        change_bytes_order(&a);
        change_bytes_order(&b);
        change_bytes_order(&c);

        ck_assert_double_eq(a, a_expected);
        ck_assert_double_eq(b, b_expected);
        ck_assert_double_eq(c, c_expected);
    }
END_TEST

Suite *
meshgen_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("MeshGen");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_refine);
    tcase_add_test(tc_core, test_is_inside_triangle);
    tcase_add_test(tc_core, test_get_height);
    tcase_add_test(tc_core, test_change_bytes_order);
    tcase_add_test(tc_core, test_refine_new_mesh);
    suite_add_tcase(s, tc_core);

    return s;
}

int
main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = meshgen_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
