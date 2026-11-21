#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "wsp.h"
#include "wsp_math.h"

static node_id *append(node_id *l, size_t n, node_id v) {
    node_id *rl = (node_id *)realloc(l, (n + 1) * sizeof(node_id));

    if (rl == NULL) {
        fprintf(stderr, "Out of memory for realloc.\n");
        exit(-1);
    }

    rl[n] = v;
    return rl;
}

static void wsp_load_nodes(const char *file, instance *inst) {
    FILE *fp = fopen(file, "r");

    if (fp == NULL) {
        fprintf(stderr, "Could not open file %s.\n", file);
        exit(-1);
    }

    fscanf(fp, "%hu", &inst->num_nodes);
    inst->num_origins = 0;
    inst->num_dests   = 0;

    inst->origin_ids = NULL;
    inst->dest_ids   = NULL;

    size_t n    = (inst->num_nodes + 1);
    inst->nodes = (node *)calloc(n, sizeof(node));

    // leave index 0 empty
    // so the node.id matches the index
    for (uint16_t i = 1; i <= inst->num_nodes; ++i) {
        node *n = &inst->nodes[i];
        fscanf(fp, "%hu %lf %lf %hu %hu", &n->id, &n->x, &n->y, &n->type,
               &n->p);

        if (wsp_is_origin(n)) {
            inst->origin_ids =
                append(inst->origin_ids, inst->num_origins++, n->id);
        }

        else if (wsp_is_destination(n)) {
            inst->dest_ids = append(inst->dest_ids, inst->num_dests++, n->id);
        }
    }

    fclose(fp);
}

static void wsp_load_edges(const char *file, instance *inst) {
    FILE *fp = fopen(file, "r");

    if (fp == NULL) {
        fprintf(stderr, "Could not open file %s.\n", file);
        exit(-1);
    }

    inst->num_blocked = 0;
    fscanf(fp, "%hu", &inst->num_edges);
    size_t n    = inst->num_edges + 1;
    inst->edges = (edge *)calloc(n, sizeof(edge));

    // leave index 0 empty
    // so the edge.id matches the index
    for (uint16_t i = 1; i <= inst->num_edges; ++i) {
        edge *e = inst->edges + i;
        fscanf(fp, "%hu %hu %hu %lf %hu %hu", &e->id, &e->i, &e->j, &e->d,
               &e->u, &e->r);

        if (wsp_is_blocked(e))
            inst->num_blocked++;
    }
    fclose(fp);
}

instance *wsp_load_instance(const char *nodes, const char *edges) {
    instance *inst = (instance *)malloc(sizeof(instance));
    wsp_load_nodes(nodes, inst);
    wsp_load_edges(edges, inst);
    // calculates the total of periods to repair all blocked edges
    inst->T = wsp_calc_T(inst);

    return inst;
}
