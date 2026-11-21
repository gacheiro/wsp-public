#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <cstdint>
#include <list>
#include <queue>
#include <utility>
#include <vector>

#include "wsp.h"

#define INF 999999

typedef std::pair<double, node_id> pi;
typedef std::vector<std::list<pi>> adj_list;
// priority queue of `pi` in ascending order
typedef std::priority_queue<pi, std::vector<pi>, std::greater<pi>>
    priority_queue;

static inline node_id *dj_make_pred_arr(size_t n) {
    return (node_id *)malloc(n * sizeof(node_id));
}

static inline double *dj_make_dist_arr(size_t n) {
    return (double *)malloc(n * sizeof(double));
}

static inline void dj_add_arc(adj_list &adj, node_id u, node_id v, double w) {
    adj[u].push_back({w, v});
}

static inline void dj_add_edge(adj_list &adj, node_id u, node_id v, double w) {
    adj[u].push_back({w, v});
    adj[v].push_back({w, u});
}

static inline bool dj_can_reach(const double *dist, uint16_t n) {
    return dist[n] < INF;
}

adj_list dj_make_adj_list(size_t n);

void dj_run_dijkstra(const adj_list &adj, node_id s, node_id *pred,
                     double *dist);

#endif
