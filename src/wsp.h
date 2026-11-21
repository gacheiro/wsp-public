#ifndef WSP_H
#define WSP_H

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_SOLUTION_SIZE 600

typedef uint16_t node_id;
typedef uint16_t edge_id;

typedef struct node {
    node_id id;
    double x, y;
    uint16_t type, p;
} node;

typedef struct edge {
    node_id id, i, j;
    double d;
    uint16_t u, r;
} edge;

typedef struct instance {
    uint16_t num_nodes, num_edges, num_origins, num_dests, num_blocked;

    node *nodes;
    edge *edges;

    uint16_t T;

    node_id *origin_ids, *dest_ids;
} instance;

typedef struct solution {
    double objective;
    size_t n; // the complete size of the solution
              // equals to |B| <= MAX_SOLUTION_SIZE

    size_t size; // current size
    edge_id order[MAX_SOLUTION_SIZE];
} solution;

instance *wsp_load_instance(const char *nodes, const char *edges);

static inline edge_id wsp_get_edge_id(const instance *inst, node_id u,
                                      node_id v) {

    for (uint16_t i = 1; i <= inst->num_edges; ++i) {
        const edge *e = &inst->edges[i];
        if ((e->i == u && e->j == v) || (e->i == v && e->j == u))
            return e->id;
    }

    // this should never happend
    fprintf(stderr, "Could not find edge [%hu, %hu]\n.", u, v);
    return 0;
}

static inline solution *wsp_new_solution(size_t n) {

    if (n > MAX_SOLUTION_SIZE)
        fprintf(stderr, "Solution size overflows allocated memory. Please "
                        "adjust MAX_SOLUTION_SIZE.\n");

    solution *s  = (solution *)malloc(sizeof(solution));
    s->objective = UINT_MAX;
    s->n         = n;
    s->size      = 0;
    return s;
}

static inline void wsp_add_to_solution(solution *s, edge_id id) {
    if (s->size >= MAX_SOLUTION_SIZE) {
        fprintf(stderr,
                "Solution overflow. Please adjust MAX_SOLUTION_SIZE.\n");
        exit(-1);
    }

    s->order[s->size++] = id;
}

static inline bool wsp_is_solution_complete(const solution *s) {
    return (s->size == s->n);
}

static inline bool wsp_is_origin(const node *n) {
    return (n->type == 1 || n->type == 2 || n->type == 3);
}

static inline bool wsp_is_destination(const node *n) { return (n->type == 4); }

static inline bool wsp_is_blocked(const edge *e) { return (e->r > 0); }

static inline void wsp_free_instance(instance *inst) {
    free(inst->nodes);
    free(inst->edges);
    free(inst->origin_ids);
    free(inst->dest_ids);
    free(inst);
}

#endif
