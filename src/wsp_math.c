#include <stdio.h>
#include <stdlib.h>

#include "dijkstra.h"
#include "spanning_tree.h"
#include "wsp.h"
#include "wsp_math.h"

static adj_list adj;
static double *dist  = NULL;
static node_id *pred = NULL;
static tree tr       = NULL;

static uint8_t *r = NULL, *ui = NULL, *uj = NULL;

static inline void reset_worktroops(uint16_t n) {
    while (n-- > 0) {
        ui[n] = uj[n] = 0;
    }
}

// check if the wt can reach an edge && the edge is not full
#define CAN_REPAIR(v, side) (tr_is_same_tree(tr, o, v) && u##side[e->id] < e->u)

// repair the edge either from side i or j
#define REPAIR(e, side)                                                        \
    do {                                                                       \
        u##side[e->id]++;                                                      \
        if (--r[e->id] == 0) {                                                 \
            repaired_edges[n_repr++] = e->id;                                  \
        }                                                                      \
    } while (0)

static inline void period_repair(const instance *inst, const solution *s) {

    // small array to store repaired edges in the period
    edge_id repaired_edges[10];
    uint8_t n_repr = 0;

    for (uint16_t k = 0; k < inst->num_origins; ++k) {
        node_id o = inst->origin_ids[k];

        for (uint16_t w = 0; w < inst->nodes[o].p; ++w) {

            for (uint16_t l = 0; l < s->size; ++l) {
                const edge *e = &inst->edges[s->order[l]];

                if (r[e->id] == 0)
                    continue;

                else if (CAN_REPAIR(e->i, i)) {
                    REPAIR(e, i);
                    break;
                } else if (CAN_REPAIR(e->j, j)) {
                    REPAIR(e, j);
                    break;
                }
            }
        }
    }

    // add the repaired edges to the tree and adj list
    for (uint8_t i = 0; i < n_repr; ++i) {
        const edge *e = &inst->edges[repaired_edges[i]];
        tr_add(tr, e->i, e->j);
        dj_add_edge(adj, e->i, e->j, e->d);
    }
}

double wsp_evaluate(const instance *inst, const solution *s, double *km) {
    double objective = 0;
    const size_t n = inst->num_nodes + 1, ne = inst->num_edges + 1;
    // the `r` array is used to store the `r` attr of each edge
    // and track its reparations
    r = (uint8_t *)malloc(ne * sizeof(uint8_t));
    // the `ui` and `uj` arrays are used to store the number of
    // wt working on each side of the edges
    ui = (uint8_t *)malloc(ne * sizeof(uint8_t));
    uj = (uint8_t *)malloc(ne * sizeof(uint8_t));

    // TODO: this startup can be moved to a separate function
    // and calculated only once as the initial state doesnt change

    // init the adjacency list
    // and pred, dist arrays
    // NOTE: remember to add 1 to make room for the artifical
    // root (node 0)
    adj  = dj_make_adj_list(n);
    pred = dj_make_pred_arr(n);
    dist = dj_make_dist_arr(n);

    // init the tree structure
    // which is used to verify if two arbitrary nodes
    // are connected
    tr = tr_make(n);

    for (uint16_t i = 1; i <= inst->num_edges; ++i) {
        const edge *e = inst->edges + i;
        // save the r attr of the edge
        r[e->id] = e->r;
        // add to adj list if the edge is availible
        if (r[e->id] == 0) {
            dj_add_edge(adj, e->i, e->j, e->d);
            tr_add(tr, e->i, e->j);
        }
    }

    // add artificial arcs from node 0 to origins
    for (int i = 0; i < inst->num_origins; ++i)
        dj_add_arc(adj, 0, inst->origin_ids[i], 0);

    // END TODO

    dj_run_dijkstra(adj, 0, pred, dist);

    //    for (int i = 1; i <= inst->num_edges; ++i)
    //        printf("r[%d] = %d\n", i, r[i]);

    for (uint16_t t = 1; t <= inst->T; ++t) {
        // make the WTs "come back to origins"
        reset_worktroops(ne);

        //         for (int i = 1; i <= inst->num_edges; ++i) {
        //             if (r[i] > 0)
        //                 printf("r[%d] = %d\n", i, r[i]);
        //         }
        //         printf("\n");

        // send the WTs to repair the blocked edges
        period_repair(inst, s);
        dj_run_dijkstra(adj, 0, pred, dist);

        double km_t = 0;
        double obj_t = wsp_period_cost(inst, dist, &km_t);
        //         printf("t = %d, obj = %lf\n", t, obj_t);

        objective += obj_t;
        *km = *km + km_t;
    //    printf("Period %d: obj %lf km %lf\n", t, obj_t, km_t);
        printf("%lf\t%lf\n", obj_t, km_t);
    }

    free(tr);
    free(r);
    free(ui);
    free(uj);
    free(pred);
    free(dist);

    return objective;
}

uint16_t wsp_calc_T(const instance *inst) {
    uint16_t i = 0;
    float Q = 0, R = 0;

    for (i = 1; i <= inst->num_nodes; ++i) {
        const node *n = &inst->nodes[i];
        if (wsp_is_origin(n))
            Q += n->p;
    }

    for (i = 1; i <= inst->num_edges; ++i) {
        const edge *e = &inst->edges[i];
        if (wsp_is_blocked(e))
            R += e->r;
    }

    return ceil(R / Q);
}

double wsp_period_cost(const instance *inst, const double *dist, double *km) {
    double cost = 0;

    for (uint16_t i = 0; i < inst->num_dests; ++i) {
        node_id d = inst->dest_ids[i];
        cost += dist[d] * inst->nodes[d].p;
        // Also, add only the distance
        *km = *km + dist[d];
    }

    return cost;
}
