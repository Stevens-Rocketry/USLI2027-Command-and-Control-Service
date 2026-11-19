/*
 *    George K Oliynyk
 *    Stevens Rocketry USLI 2027
 */

#ifndef TEST_H
#define TEST_H

/* DEFINES */

#define ID_TESTSTATE 0
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
    {"test"},
    {"test"}
};

#endif //TEST_H
