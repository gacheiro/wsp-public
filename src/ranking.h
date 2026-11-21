#ifndef RANKING_H
#define RANKING_H

#include "wsp.h"

typedef struct {
    edge_id id;
    uint16_t count;
} edge_rank;

edge_rank *rk_rank_edges(const instance *inst);

solution *rk_ranking(const instance *inst);

#endif
