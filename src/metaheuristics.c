#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "localsearch.h"
#include "metaheuristics.h"
#include "ranking.h"
#include "wsp.h"
#include "wsp_math.h"

/* Pops and returns the nth != 0 element from the array c.
 * the element is returned and replaced by 0 in the array */
static inline edge_id pop_nth(edge_id *c, uint16_t nth, uint16_t total) {
    uint16_t n = 0, i = 0;

    while (n != nth && n < total)
        if (c[i++] != 0)
            ++n;

    edge_id id = c[n];
    // marks the element as removed
    c[n] = 0;
    return id;
}

static solution *construction_phase(const instance *inst, double alpha) {

    uint16_t nb_candidates = inst->num_blocked, added_candidates = 0;
    edge_id *candidates = (edge_id *)malloc(nb_candidates * sizeof(edge_id));
    /* Apply the greedy strategy of the ranking heuristic.
     * Counts the number of occurrencies of each edge in
     * the SPs from origins to destinations. Edges are ranked
     * based on their counter. */
    edge_rank *ranked_edges = rk_rank_edges(inst);
    for (uint16_t i = 0; i < inst->num_edges + 1; ++i) {
        const edge *e = &inst->edges[ranked_edges[i].id];
        // add only blocked edges to the solution
        if (wsp_is_blocked(e))
            candidates[added_candidates++] = e->id;
    }

    solution *s = wsp_new_solution(nb_candidates);

    while (nb_candidates > 0) {
        uint16_t p   = 1 + alpha * (nb_candidates - 1);
        uint16_t nth = rand() % p;

        edge_id c = pop_nth(candidates, nth, nb_candidates);
        wsp_add_to_solution(s, c);
        nb_candidates--;
    }

    assert(wsp_is_solution_complete(s));
    free(candidates);
    free(ranked_edges);

    return s;
}

/* GRASP implementation with the adapted ranking heuristic as construction phase
 */
double meta_grasp(const instance *inst, double alpha, uint16_t nb_iterations) {

    if (alpha < 0 || alpha > 1) {
        fprintf(stderr, "alpha should be in [0, 1].\n");
        exit(EXIT_FAILURE);
    }

    solution *s           = NULL;
    double best_objective = INT_MAX;

    while (nb_iterations-- > 0) {
        s = construction_phase(inst, alpha);
        ls_localsearch(inst, s);

        if (s->objective < best_objective)
            best_objective = s->objective;

        free(s);
    }

    return best_objective;
}

static inline void pertubate(solution *s, double pertubation_str) {
    uint16_t nb_swaps = s->size * (pertubation_str / 2);

    while (nb_swaps-- > 0) {
        uint16_t i = rand() % s->size;
        uint16_t j = rand() % s->size;
        LS_SWAP(s->order[i], s->order[j]);
    }
}

double meta_ils(const instance *inst, double pertubation_str,
                uint16_t nb_iterations) {

    if (pertubation_str < 0 || pertubation_str > 1) {
        fprintf(stderr, "pertubation strength should be in [0, 1].\n");
        exit(EXIT_FAILURE);
    }

    solution *s = NULL, ps;
    double km   = 0;
    s           = rk_ranking(inst);
    ls_localsearch(inst, s);

    while (nb_iterations-- > 0) {

        /* Shallow copy solution s into ps.
         * Make sure struct solution has no pointers. */
        ps = *s;
        pertubate(&ps, pertubation_str);
        // evaluate the pertubated solution
        ps.objective = wsp_evaluate(inst, &ps, &km);
        ls_localsearch(inst, &ps);

        if (ps.objective < s->objective) {
            *s = ps; // copy ps into s
        }
        
        printf("ILS: nb_iteration %d: %lf obj\n", nb_iterations, s->objective);
    }

    double best_objective = s->objective;
    free(s);
    return best_objective;
}
