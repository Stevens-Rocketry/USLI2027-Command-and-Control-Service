/*
 *    George K Oliynyk
 *    Stevens Rocketry USLI 2027
 */

#ifndef TEST_H
#define TEST_H

/* DEFINES */

#define ID_TESTSTATE 1 /* Never use 0 for ID */
#define PRIORITY_TESTSTATE 0

/* INCLUDES */

#include "../state.h"

/* FUNCTIONS */

bool isActive();

/* VARS */

static const State TestState = {
    ID_TESTSTATE,
    &isActive,
    PRIORITY_TESTSTATE,
    {"test.service"},
    {0}
};

#endif //TEST_H
