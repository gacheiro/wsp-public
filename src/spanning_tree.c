#include <stdio.h>
#include <stdlib.h>

#include "spanning_tree.h"

tree tr_make(size_t n) {
    tree parent = (uint16_t *)malloc(n * sizeof(uint16_t));

    if (parent == NULL) {
        fprintf(stderr, "Out of memory for tree\n.");
        exit(-1);
    }

    for (size_t i = 0; i < n; ++i)
        parent[i] = i;

    return parent;
}

static uint16_t find(const tree parent, node_id u) {
    while (u != parent[u])
        u = parent[u];
    return u;
}

bool tr_add(tree parent, node_id u, node_id v) {
    int ru = find(parent, u), rv = find(parent, v);

    if (ru != rv) {
        parent[ru] = rv;
        return true;
    }

    return false;
}

bool tr_is_same_tree(const tree parent, node_id u, node_id v) {
    return find(parent, u) == find(parent, v);
}
