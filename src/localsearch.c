#include <stdio.h>

#include "localsearch.h"
#include "wsp.h"
#include "wsp_math.h"

static bool scan_neighborhood(const instance *inst, solution *s) {
    for (size_t i = 1; i < s->size - 1; ++i) {
        for (size_t j = i + 1; j < s->size; ++j) {
            LS_SWAP(s->order[i], s->order[j]);

            double km = 0;
            double objective = wsp_evaluate(inst, s, &km);
            printf("%lu, %lu found %lf, km=%lf\n", i, j, objective, km);
            if (objective < s->objective) {
                s->objective = objective;
                return true;
            } else {
                // undo the swap
                LS_SWAP(s->order[i], s->order[j]);
            }
        }
    }

    return false;
}

void ls_localsearch(const instance *inst, solution *s) {
    for (size_t i = 1; scan_neighborhood(inst, s); ++i)
        printf("LS iteration %lu: %lf\n", i, s->objective);
}
