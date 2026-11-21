#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "dijkstra.h"
#include "ranking.h"
#include "wsp.h"
#include "wsp_math.h"

static int cmp_edge_rank(const void *a, const void *b) {
    return ((edge_rank *)b)->count - ((edge_rank *)a)->count;
}

static inline void count_edges(const instance *inst, edge_rank *rank,
                               const node_id *pred) {

    for (uint16_t d = 0; d < inst->num_dests; ++d) {

        node_id node = inst->dest_ids[d];
        node_id prev = pred[node];

        // extract and count edges in sps from d back to source
        while (prev != 0) {
            edge_id id = wsp_get_edge_id(inst, prev, node);
            rank[id].count++;

            node = prev;
            prev = pred[node];
        }
    }
}

static inline void sort_edges(size_t n, edge_rank *rank) {
    qsort(rank, n, sizeof(edge_rank), cmp_edge_rank);
}

edge_rank *rk_rank_edges(const instance *inst) {
    size_t n        = inst->num_edges + 1;
    edge_rank *rank = (edge_rank *)malloc(n * sizeof(edge_rank));

    for (uint16_t i = 0; i < n; ++i) {
        rank[i].id    = i;
        rank[i].count = 0;
    }

    adj_list adj  = dj_make_adj_list(n);
    node_id *pred = dj_make_pred_arr(n);
    double *dist  = dj_make_dist_arr(n);

    // add edges to adjacency list using the `r` attr as cos
    for (uint16_t i = 1; i < inst->num_edges + 1; ++i) {
        const edge *e = &inst->edges[i];
        dj_add_edge(adj, e->i, e->j, e->r);
    }

    for (uint16_t i = 0; i < inst->num_origins; ++i) {
        // calculate the sps from origin
        dj_run_dijkstra(adj, inst->origin_ids[i], pred, dist);
        // count the occurrence of each edge in the sps from destinations
        count_edges(inst, rank, pred);
    }

    sort_edges(n, rank);

    free(pred);
    free(dist);
    return rank;
}

solution *rk_ranking(const instance *inst) {
    solution *s     = wsp_new_solution(inst->num_blocked);
    edge_rank *rank = rk_rank_edges(inst);

    for (uint16_t i = 0; i < inst->num_edges + 1; ++i) {
        const edge *e = &inst->edges[rank[i].id];
        // add only blocked edges to solution
        if (wsp_is_blocked(e))
            wsp_add_to_solution(s, e->id);
    }

    assert(wsp_is_solution_complete(s));

    // use the evaluate function from the wsp_math module
    // to calculate the objective value.
    // this is a trick that can be used with the ranking heuristic
    // as the evaluate function works exacly the same way from this point on
    double km = 0;
    s->objective = wsp_evaluate(inst, s, &km);

    free(rank);
    return s;
}
