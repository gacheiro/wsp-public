#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "dijkstra.cpp"
#include "minunit.h"

static const char *test_make_adj() {
    adj_list adj = dj_make_adj_list(100);

    assert(adj.size() == 100);
    return 0;
}

static const char *test_make_pred() {
    uint16_t *pred = NULL;
    pred           = dj_make_pred_arr(100);

    assert(pred != NULL);

    free(pred);
    return 0;
}

static const char *test_make_dist() {
    double *dist = NULL;
    dist         = dj_make_dist_arr(100);

    assert(dist != NULL);

    free(dist);
    return 0;
}

static const char *test_add_arc() {
    adj_list adj = dj_make_adj_list(100);
    pi p         = {10.5, 1};
    dj_add_arc(adj, 0, 1, 10.5);

    for (int i = 1; i < 100; ++i) {
        assert(adj[i].size() == 0);
    }

    assert(adj[0].size() == 1);
    assert(adj[0].front() == p);

    return 0;
}

static const char *test_add_edge() {
    adj_list adj = dj_make_adj_list(100);
    pi p1        = {10.5, 1}; // arc from 0 to 1
    pi p2        = {10.5, 0}; // arc from 1 to 0
    dj_add_edge(adj, 0, 1, 10.5);

    for (int i = 2; i < 100; ++i) {
        assert(adj[i].size() == 0);
    }

    assert(adj[0].size() == 1);
    assert(adj[1].size() == 1);
    assert(adj[0].front() == p1);
    assert(adj[1].front() == p2);

    return 0;
}

static const char *test_run__no_path() {
    /* test the dijkstra algorithm in a
    graph with no path to nodes */
    size_t n       = 10;
    adj_list adj   = dj_make_adj_list(n);
    uint16_t *pred = dj_make_pred_arr(n);
    double *dist   = dj_make_dist_arr(n);

    dj_run_dijkstra(adj, 0, pred, dist);

    assert(dist[0] == 0);
    for (size_t i = 1; i < n; ++i) {
        assert(dist[i] == INF);
    }

    free(pred);
    free(dist);
    return 0;
}

static const char *test_run() {
    size_t n       = 11;
    adj_list a     = dj_make_adj_list(n);
    uint16_t *pred = dj_make_pred_arr(n);
    double *dist   = dj_make_dist_arr(n);

    // add the edges from instance g1_1n10r40B15
    dj_add_edge(a, 1, 2, 4);
    dj_add_edge(a, 1, 3, 8);
    dj_add_edge(a, 1, 4, 3);
    dj_add_edge(a, 2, 3, 6);
    dj_add_edge(a, 2, 5, 4);
    dj_add_edge(a, 2, 6, 6);
    dj_add_edge(a, 3, 4, 6);
    dj_add_edge(a, 3, 6, 7);
    dj_add_edge(a, 3, 7, 5);
    dj_add_edge(a, 4, 7, 2);
    dj_add_edge(a, 4, 10, 10);
    dj_add_edge(a, 5, 6, 2);
    dj_add_edge(a, 5, 8, 3);
    dj_add_edge(a, 6, 7, 3);
    dj_add_edge(a, 6, 8, 4);
    dj_add_edge(a, 6, 9, 5);
    dj_add_edge(a, 7, 10, 5);
    dj_add_edge(a, 7, 9, 4);
    dj_add_edge(a, 8, 9, 9);
    dj_add_edge(a, 9, 10, 6);

    dj_run_dijkstra(a, 1, pred, dist);

    double expect_dist[] = {INF, 0, 4, 8, 3, 8, 8, 5, 11, 9, 10};
    for (size_t i = 0; i < n; ++i) {
        assert(dist[i] == expect_dist[i]);
    }

    uint16_t expect_pred[] = {0, 0, 1, 1, 1, 2, 7, 4, 5, 7, 7};
    for (size_t i = 0; i < n; ++i) {
        assert(pred[i] == expect_pred[i]);
    }

    free(pred);
    free(dist);
    return 0;
}

const char *test_dijkstra() {
    mu_run_test(test_make_adj);
    mu_run_test(test_make_pred);
    mu_run_test(test_make_dist);
    mu_run_test(test_add_arc);
    mu_run_test(test_add_edge);

    mu_run_test(test_run__no_path);
    mu_run_test(test_run);

    return 0;
}
