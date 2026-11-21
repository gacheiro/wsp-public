#include "localsearch.h"
#include "metaheuristics.h"
#include "ranking.h"
#include "wsp.h"

int main() {
    const instance *inst = wsp_load_instance("instances/vertices.txt",
                                             "instances/g1/3n10r45B20.txt");
    const float pertubation_str = 0.3;
    const int nb_iterations     = 200;

    printf("Running ils\n");
    double objective = meta_ils(inst, pertubation_str, nb_iterations);
    printf("ILS: %lf\n", objective);

    wsp_free_instance((instance *)inst);
    return 0;
}
