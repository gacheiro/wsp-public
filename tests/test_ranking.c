#include <stdlib.h>

#include "dijkstra.h"
#include "minunit.h"
#include "ranking.c"
#include "wsp.h"

static const char *test_count_edges() {
    const instance *inst = wsp_load_instance("instances/vertices.txt",
                                             "instances/g1/1n10r40B15.txt");
    edge_rank *rank      = (edge_rank *)malloc(21 * sizeof(edge_rank));

    for (uint16_t i = 0; i < 21; ++i) {
        rank[i].id    = i;
        rank[i].count = 0;
    }

    // pred array calculated with dijkstra source node 1
    const node_id pred_1[] = {0, 0, 1, 1, 1, 2, 7, 4, 5, 7, 7};
    count_edges(inst, rank, pred_1);

    // pred array calculated with dijkstra source node 8
    const node_id pred_8[] = {0, 2, 5, 6, 7, 8, 8, 6, 0, 8, 7};
    count_edges(inst, rank, pred_8);

    const uint16_t expected_count[] = {0, 1, 0, 2, 0, 1, 0, 0, 0, 0, 2,
                                       0, 0, 1, 2, 2, 0, 0, 0, 0, 0};

    for (uint16_t i = 0; i < 21; ++i) {
        assert(rank[i].count == expected_count[i]);
    }

    free(rank);
    wsp_free_instance((instance *)inst);
    return 0;
}

static const char *test_sort_edges() {
    edge_rank *rank = (edge_rank *)malloc(21 * sizeof(edge_rank));

    for (uint16_t i = 0; i < 21; ++i) {
        rank[i].id    = i;
        rank[i].count = i; // set count as i
    }

    sort_edges(21, rank);
    for (uint16_t i = 0; i < 21; ++i) {
        assert(rank[i].count == 20 - i);
    }

    free(rank);
    return 0;
}

static const char *test_rank_edges() {
    const instance *inst = wsp_load_instance("instances/vertices.txt",
                                             "instances/g1/1n10r40B15.txt");
    solution *s          = wsp_new_solution(inst->num_blocked);

    edge_rank *rank = rk_rank_edges(inst);
    assert(rank != NULL);

    for (uint16_t i = 0; i < inst->num_edges + 1; ++i) {
        const struct edge *e = &inst->edges[rank[i].id];
        // add only blocked edges to solution
        if (wsp_is_blocked(e)) {
            wsp_add_to_solution(s, e->id);
        }
    }

    // ranks of edges 2, 12, 3, 17 = 2, 2, 1, 1
    // other edges have rank 0
    edge_id expected[] = {2, 12, 3, 17, 1, 4, 5, 6, 7, 9, 10, 13, 15, 16, 20};

    assert(wsp_is_solution_complete(s));
    for (uint16_t i = 0; i < s->size; ++i)
        assert(s->order[i] == expected[i]);

    free(rank);
    wsp_free_instance((instance *)inst);
    free(s);

    return 0;
}

static const char *test_ranking_h() {
    const instance *inst = wsp_load_instance("instances/vertices.txt",
                                             "instances/g1/3n10r45B20.txt");
    solution *s          = rk_ranking(inst);

    //    printf("Objective[r] = %lf\n", s.objective);
    assert(s->objective == 170622);

    wsp_free_instance((instance *)inst);
    free(s);

    return 0;
}

static const char *test_ranking() {
    mu_run_test(test_count_edges);
    mu_run_test(test_sort_edges);
    mu_run_test(test_rank_edges);
    mu_run_test(test_ranking_h);

    return 0;
}
