#include <stdlib.h>

#include "metaheuristics.c"
#include "minunit.h"
#include "wsp.h"

static const char *test_grasp() {
    const instance *inst = wsp_load_instance("instances/vertices.txt",
                                             "instances/g1/3n10r45B20.txt");
    double objective     = meta_grasp(inst, 0.9, 5);

    printf("GRASP: %lf\n", objective);

    wsp_free_instance((instance *)inst);
    return 0;
}

static const char *test_ils() {
    const instance *inst = wsp_load_instance("instances/vertices.txt",
                                             "instances/g1/3n10r45B20.txt");
    double objective     = meta_ils(inst, 0.3, 5);

    printf("ILS: %lf\n", objective);

    wsp_free_instance((instance *)inst);
    return 0;
}

static const char *test_meta() {
    mu_run_test(test_grasp);
    mu_run_test(test_ils);

    return 0;
}
