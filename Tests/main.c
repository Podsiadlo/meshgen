#include <check.h>

#include "mesh.h"
#include "refinement.h"

#include <stdint.h>
#include <stdlib.h>

Suite *
meshgen_suite(void);

START_TEST(test_refine_new_mesh)
{

    // Simple 5x5 map with all heights zero except for one with 100m
    double **map = (double **)malloc(5 * sizeof(double *));
    for (int i = 0; i < 5; ++i) {
        map[i] = (double *)malloc(5 * sizeof(double));
        for (int j = 0; j < 5; ++j) {
            map[i][j] = 0;
        }
    }
    map[2][3] = 100;

    // Generate the initial mesh
    struct mesh *local_mesh = generate_mesh((const double **)map, 5, 5);

    refine_new_mesh(local_mesh);

    ck_assert_int_eq(local_mesh->counter, 13);
}
END_TEST

START_TEST(test_refine)
{

    double **map = (double **)malloc(5 * sizeof(double *));
    for (int i = 0; i < 5; ++i) {
        map[i] = (double *)malloc(5 * sizeof(double));
        for (int j = 0; j < 5; ++j) {
            map[i][j] = 0;
        }
    }

    // Generate the initial mesh
    struct mesh *local_mesh = generate_mesh((const double **)map, 5, 5);

    struct triangle *triangle_to_refine = &(local_mesh->triangles[0]);

    refine(triangle_to_refine, local_mesh);

    ck_assert_int_eq(local_mesh->counter, 4);

    refine(triangle_to_refine, local_mesh);

    ck_assert_int_eq(local_mesh->counter, 5);

    refine(triangle_to_refine, local_mesh);

    ck_assert_int_eq(local_mesh->counter, 8);

    refine(triangle_to_refine, local_mesh);

    ck_assert_int_eq(local_mesh->counter, 13);
}
END_TEST

START_TEST(test_is_inside_triangle)
    {

        double step = 1;
        int lower_boundary = 1;
        double upper_boundary = 6.5;
        struct triangle * triangle = (struct triangle *)malloc(sizeof(struct triangle));

        triangle->vertices[0].x = lower_boundary;
        triangle->vertices[0].y = lower_boundary;
        triangle->vertices[1].x = upper_boundary;
        triangle->vertices[1].y = upper_boundary;
        triangle->vertices[2].x = upper_boundary;
        triangle->vertices[2].y = lower_boundary;

        int points_inside = 0;
        for (double i = 0; i <= floor(upper_boundary); i+= step) {
            for (double j = 0; j <= floor(upper_boundary); j+= step) {
                points_inside += is_inside_triangle(i, j, triangle) ? lower_boundary : 0;
            }
        }

        double n = floor((upper_boundary) / step);
        double expected = (lower_boundary + (n - 1) / 2) * n;

        ck_assert_int_eq(points_inside, expected);
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

    tcase_add_test(tc_core, test_refine_new_mesh);
    tcase_add_test(tc_core, test_refine);
    tcase_add_test(tc_core, test_is_inside_triangle);
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
