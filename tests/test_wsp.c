#include <string.h>

#include "minunit.h"
#include "wsp.c"
#include "wsp_math.c"

static const char *test_load() {
    const char *fnodes = "instances/vertices.txt",
               *fedges = "instances/g1/1n10r40B15.txt";

    const instance *inst = wsp_load_instance(fnodes, fedges);

    assert(inst->num_nodes == 10);

    // check nodes
    const node nodes[] = {{0, 0, 0, 0, 0}, // dummy node
                          {1, 6.6, 34.6, 2, 2},  {2, 54.76, 434.7, 0, 0},
                          {3, 54.6, 78.2, 0, 0}, {4, 98.2, 23.6, 4, 6},
                          {5, 6.6, 34.6, 4, 4},  {6, 54.76, 434.7, 0, 0},
                          {7, 54.6, 78.2, 4, 3}, {8, 98.2, 23.6, 1, 2},
                          {9, 54.6, 78.2, 0, 0}, {10, 98.2, 23.6, 0, 0}};

    for (int i = 1; i <= 10; ++i) {
        assert(nodes[i].id == inst->nodes[i].id);
        assert(nodes[i].x == inst->nodes[i].x);
        assert(nodes[i].y == inst->nodes[i].y);
        assert(nodes[i].type == inst->nodes[i].type);
        assert(nodes[i].p == inst->nodes[i].p);
    }

    assert(inst->num_edges == 20);
    assert(inst->num_blocked == 15);
    assert(inst->T == 10);

    // check edges
    const edge edges[] = {
        {0, 0, 0, 0.0, 0, 0}, // dummy edge
        {1, 1, 2, 4.0, 1, 2},  {2, 1, 3, 8.0, 1, 3},    {3, 1, 4, 3.0, 2, 3},
        {4, 2, 3, 6.0, 1, 5},  {5, 2, 5, 4.0, 1, 3},    {6, 2, 6, 6.0, 1, 6},
        {7, 3, 4, 6.0, 1, 3},  {8, 3, 6, 7.0, 1, 0},    {9, 3, 7, 5.0, 1, 3},
        {10, 4, 7, 2.0, 2, 2}, {11, 4, 10, 10.0, 1, 0}, {12, 5, 6, 2.0, 2, 1},
        {13, 5, 8, 3.0, 1, 3}, {14, 6, 7, 3.0, 2, 0},   {15, 6, 8, 4.0, 1, 1},
        {16, 6, 9, 5.0, 1, 1}, {17, 7, 10, 5.0, 1, 1},  {18, 7, 9, 4.0, 1, 0},
        {19, 8, 9, 9.0, 1, 0}, {20, 9, 10, 6.0, 1, 3}};

    for (int i = 1; i <= 20; ++i) {
        assert(edges[i].id == inst->edges[i].id);
        assert(edges[i].i == inst->edges[i].i);
        assert(edges[i].j == inst->edges[i].j);
        assert(edges[i].d == inst->edges[i].d);
        assert(edges[i].u == inst->edges[i].u);
        assert(edges[i].r == inst->edges[i].r);
    }

    // check origins and destinations
    assert(inst->num_origins == 2);
    assert(inst->num_dests == 3);

    assert(1 == inst->origin_ids[0]);
    assert(8 == inst->origin_ids[1]);

    assert(4 == inst->dest_ids[0]);
    assert(5 == inst->dest_ids[1]);
    assert(7 == inst->dest_ids[2]);

    wsp_free_instance((instance *)inst);
    return 0;
}

static const char *test_solution() {
    solution *s = wsp_new_solution(5); // sol with 5 blocked edges

    assert(!wsp_is_solution_complete(s));
    wsp_add_to_solution(s, 1);
    wsp_add_to_solution(s, 2);
    wsp_add_to_solution(s, 3);
    wsp_add_to_solution(s, 4);
    assert(!wsp_is_solution_complete(s));
    wsp_add_to_solution(s, 5);
    assert(wsp_is_solution_complete(s));

    free(s);
    return 0;
}

static const char *test_calc_T() {
    const instance *inst = wsp_load_instance("instances/vertices.txt",
                                             "instances/g1/1n10r40B5.txt");

    assert(wsp_calc_T(inst) == 10);

    const instance *inst2 = wsp_load_instance("instances/vertices.txt",
                                              "instances/g1/1n10r45B5.txt");
    assert(wsp_calc_T(inst2) == 12);

    wsp_free_instance((instance *)inst);
    wsp_free_instance((instance *)inst2);
    return 0;
}

static const char *test_evaluate() {

    const instance *inst = wsp_load_instance("instances/vertices.txt",
                                             "instances/g1/2n10r45B20.txt");
    solution *s          = wsp_new_solution(20);

    for (size_t i = 1; i <= inst->num_edges; ++i) {
        const edge *e = inst->edges + i;
        if (wsp_is_blocked(e))
            wsp_add_to_solution(s, e->id);
    }
    assert(wsp_is_solution_complete(s));

    double result = wsp_evaluate(inst, s);
    assert(result == 420360);

    wsp_free_instance((instance *)inst);
    free(s);
    return 0;
};

static const char *test_wsp() {
    mu_run_test(test_load);
    mu_run_test(test_solution);

    // test wsp_math
    mu_run_test(test_calc_T);
    mu_run_test(test_evaluate);

    return 0;
}
