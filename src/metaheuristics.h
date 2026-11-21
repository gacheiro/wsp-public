#ifndef META_H
#define META_H

#define ILS_PERTUBATION_STR 30

#include <stdint.h>

#include "wsp.h"

double meta_grasp(const instance *inst, double alpha, uint16_t nb_iterations);

double meta_ils(const instance *inst, double pertubation_str,
                uint16_t nb_iterations);
#endif
