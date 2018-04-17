#include <check.h>

#include "mesh.h"
#include "output.h"
#include "refinement.h"

#include <stdint.h>
#include <stdlib.h>

Suite *
meshgen_suite(void);

START_TEST(test_refine_new_mesh)
{

    // Simple 5x5 map with all heights zero except for one with 100m
    short **map = (short **)malloc(5 * sizeof(short *));
    for (int i = 0; i < 5; ++i) {
        map[i] = (short *)malloc(5 * sizeof(short));
        for (int j = 0; j < 5; ++j) {
            map[i][j] = 0;
        }
    }
    map[2][3] = 100;

    // Generate the initial mesh
    struct mesh *local_mesh = generate_mesh((const short **)map, 5, 5);

    refine_new_mesh(local_mesh);

    ck_assert_int_eq(local_mesh->counter, 13);
}
END_TEST

START_TEST(test_refine)
{

    short **map = (short **)malloc(5 * sizeof(short *));
    for (int i = 0; i < 5; ++i) {
        map[i] = (short *)malloc(5 * sizeof(short));
        for (int j = 0; j < 5; ++j) {
            map[i][j] = 0;
        }
    }

    // Generate the initial mesh
    struct mesh *local_mesh = generate_mesh((const short **)map, 5, 5);

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
