#include <stdio.h>

#include "wsp.h"

int main() {
    const char *nf = "instances/vertices.txt";
    const char *ef = "instances/g1/0n10r45B20.txt";

    const instance *inst = wsp_load_instance(nf, ef);
    printf("Instance (%s, %s) loaded.\n", nf, ef);
    printf("%hu nodes, %hu edges.\n", inst->num_nodes, inst->num_edges);

    printf("%hu origins, %hu destinations.\n", inst->num_origins,
           inst->num_dests);
    printf("Origins are:\n");
    for (uint16_t i = 0; i < inst->num_origins; ++i)
        printf("%hu ", inst->origin_ids[i]);

    printf("\nDestinations are:\n");
    for (uint16_t i = 0; i < inst->num_dests; ++i)
        printf("%hu ", inst->dest_ids[i]);
    printf("\n");

    return 0;
}
