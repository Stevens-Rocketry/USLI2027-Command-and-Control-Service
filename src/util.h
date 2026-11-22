/*
 *    George K Oliynyk
 *    Stevens Rocketry USLI 2027
 */

#ifndef CCSERVICE_UTIL_H
#define CCSERVICE_UTIL_H

/* DEFINES */

#define _cleanup_(f) __attribute__((cleanup(f)))

/* FUNCTIONS */

static inline void freep(void *p) { free(*(void **)p); }

#endif // CCSERVICE_UTIL_H