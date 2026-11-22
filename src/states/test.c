/*
 *    George K Oliynyk
 *    Stevens Rocketry USLI 2027
 */

#include "test.h"

static bool isActive(void) {
  return true;
}

const State TestState = {
  .StateID         = ID_TESTSTATE,
  .isActive        = &isActive,
  .Priority        = PRIORITY_TESTSTATE,
  .ServicesToStart = {"test.service"},
  .ServicesToStop  ={0}
};