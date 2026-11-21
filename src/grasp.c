#include "localsearch.h"
#include "metaheuristics.h"
#include "ranking.h"
#include "wsp.h"

int main() {
    const instance *inst = wsp_load_instance("instances/vertices.txt",
                                             "instances/g2/9n10r45B20.txt");
    const float alpha       = 0.9;
    const int nb_iterations = 200;

    double objective = meta_grasp(inst, alpha, nb_iterations);
    printf("GRASP: %lf\n", objective);

    wsp_free_instance((instance *)inst);
    return 0;
}
