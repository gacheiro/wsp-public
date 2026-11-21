#include <stdlib.h>
#include <string.h>

#include "dijkstra.h"

// creates and returns an adjacency list
adj_list dj_make_adj_list(size_t n) {
    std::list<pi> l;
    adj_list adj(n, l);
    return adj;
}

void dj_run_dijkstra(const adj_list &adj, node_id s, node_id *pred,
                     double *dist) {
    for (size_t i = 0; i < adj.size(); ++i) {
        pred[i] = 0;
        dist[i] = INF;
    }
    dist[s] = 0;

    priority_queue pq;
    pq.push(std::make_pair(0, s));

    while (!pq.empty()) {
        node_id u = pq.top().second;
        pq.pop();

        for (const auto it : adj[u]) {
            double w   = it.first;
            node_id v  = it.second;

            if (dist[v] > dist[u] + w) {
                dist[v] = dist[u] + w;
                pred[v] = u;
                pq.push({dist[v], v});
            }
        }
    }
}
