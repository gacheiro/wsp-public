#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "minunit.h"
#include "spanning_tree.c"

static const char *test_make() {
    tree tr = tr_make(100);

    assert(tr != NULL);
    for (size_t i = 0; i < 100; ++i)
        assert(tr[i] == i);

    free(tr);
    return 0;
}

static const char *test_add() {
    size_t n = 11;
    tree tr  = tr_make(n);

    // add the availible edges from instance g1_1n10r40B15
    tr_add(tr, 3, 6);
    tr_add(tr, 4, 10);
    tr_add(tr, 6, 7);
    tr_add(tr, 7, 9);
    tr_add(tr, 8, 9);

    assert(tr_is_same_tree(tr, 3, 3));
    assert(tr_is_same_tree(tr, 3, 6));
    assert(tr_is_same_tree(tr, 3, 7));
    assert(tr_is_same_tree(tr, 3, 8));

    // test isolated nodes
    assert(!tr_is_same_tree(tr, 1, 3));
    assert(!tr_is_same_tree(tr, 5, 8));
    assert(!tr_is_same_tree(tr, 1, 8));

    // test a separate tree
    assert(tr_is_same_tree(tr, 4, 10));
    assert(!tr_is_same_tree(tr, 3, 4));

    free(tr);
    return 0;
}

const char *test_spanning_tree() {
    mu_run_test(test_make);
    mu_run_test(test_add);

    return 0;
}
