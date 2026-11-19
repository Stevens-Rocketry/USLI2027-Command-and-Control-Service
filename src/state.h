/*
 *    George K Oliynyk
 *    Stevens Rocketry USLI 2027
 */

#ifndef STATE_H
#define STATE_H

/* DEFINES */

#define MAXSERVICES 5

/* INCLUDES */

#include <stdint.h>

/* VARS */

typedef struct State {
    uint8_t StateID;
    bool (*isActive)(void);
    uint8_t Priority;
    const char* ServicesToStart[MAXSERVICES];
    const char* ServicesToStop[MAXSERVICES];
} State;

#endif //STATE_H
