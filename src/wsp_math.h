#ifndef WSP_MATH_H
#define WSP_MATH_H

#include <math.h>
#include <stdint.h>

#include "wsp.h"

uint16_t wsp_calc_T(const instance *inst);
double wsp_evaluate(const instance *inst, const solution *s, double *km);
double wsp_period_cost(const instance *inst, const double *dist, double *km);

#endif
