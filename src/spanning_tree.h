#ifndef SPANING_TREE_H
#define SPANING_TREE_H

#include <stdbool.h>
#include <stdint.h>

#include "wsp.h"

typedef uint16_t *tree;

tree tr_make(size_t n);

bool tr_add(tree parent, node_id u, node_id v);

bool tr_is_same_tree(const tree parent, node_id u, node_id v);
#endif
