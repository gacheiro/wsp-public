#ifndef LOCALSEARCH_H
#define LOCALSEARCH_H

#include "wsp.h"

#define LS_SWAP(a, b)                                                          \
    do {                                                                       \
        a ^= b;                                                                \
        b ^= a;                                                                \
        a ^= b;                                                                \
    } while (0)

void ls_localsearch(const instance *inst, solution *s);

#endif
