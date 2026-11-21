#include "localsearch.c"
#include "minunit.h"

static const char *test_SWAP_MACRO() {
    uint16_t a[] = {0, 1, 2, 3, 4, 5};

    LS_SWAP(a[1], a[5]);
    assert(a[1] == 5);
    assert(a[5] == 1);

    LS_SWAP(a[1], a[5]);
    assert(a[1] == 1);
    assert(a[5] == 5);

    return 0;
}

static const char *test_ls() {
    const instance *inst = wsp_load_instance("instances/vertices.txt",
                                             "instances/g1/1n10r40B5.txt");

    solution *s = wsp_new_solution(5);

    for (size_t i = 1; i <= inst->num_edges; ++i) {
        const edge *e = inst->edges + i;
        if (wsp_is_blocked(e))
            wsp_add_to_solution(s, e->id);
    }

    ls_localsearch(inst, s);
    assert(s->objective == 502);

    wsp_free_instance((instance *)inst);
    free(s);
    return 0;
}

static const char *test_localsearch() {
    mu_run_test(test_SWAP_MACRO);
    mu_run_test(test_ls);

    return 0;
}
