/*
 *    George K Oliynyk
 *    Stevens Rocketry USLI 2027
 *
 *    This file contains the definitions actually filled in for when a service
 *    is built. For the most part, if you are making a new service, you should
 *    not touch any of the other provided files unless you know what you are
 *    doing. Which hopefully you do.
 */

#include "service.h"

#include <stdlib.h>
#include <systemd/sd-bus.h>
#include <errno.h>

#include "state.h"
#include "states/test.h"
#include "sdbus.h"

#define EXIT_BUSY 2

sd_bus *SDBus;
/* It is recommended that states are put in order of lowest priority to highest
 * within the array.
 */
static const State *const States[] = {
  &TestState
};

static int start_stop_service(const char* serviceName, bool active) {
  char* activeState;
  uint32_t jobID;
  int r;

  r = serviceStatus(serviceName, SDBus, &activeState);
  if (r != EXIT_SUCCESS) {
    fprintf(stderr, "[service_loop]: Failed to acquire service status for %s: %s\n", serviceName, strerror(-r));
    return r;
  }

  if (!strcmp(activeState, active ? "active" : "inactive")) {
    free(activeState);
    return EXIT_SUCCESS;
  }
  free(activeState);

  r = serviceJobID(serviceName, SDBus, &jobID);
  if (r != EXIT_SUCCESS) {
    fprintf(stderr, "[service_loop]: Failed to acquire service(%s)'s current job ID: %s\n", serviceName, strerror(-r));
    return r;
  }

  if (jobID) {
    return EXIT_BUSY;
  }

  fprintf(stderr, "[service_loop]: %s service %s.\n", active ? "Starting" : "Stopping", serviceName);

  if (active) {
    return startService(serviceName, SDBus);
  }

  return stopService(serviceName, SDBus);
}

static int handle_services(const char *const services[], const bool start) {
  int r = 0;

  for (int i = 0; i < MAXSERVICES; i++) {
    if (!services[i]) {
      continue;
    }

    r = start_stop_service(services[i], start);

    if (r == EXIT_BUSY) {
      fprintf(stderr, "[service_loop]: Attempting to %s service %s, but "
                      "service had jobs queued. Waiting until next iteration.\n", start ? "start" : "stop", services[i]);
    } else if (r != EXIT_SUCCESS) {
      fprintf(stderr, "[service_loop]: Failed to %s service %s!\n", start ? "start" : "stop", services[i]);
      return -EIO;
    }
  }
  return EXIT_SUCCESS;
}

static int start_services(const char *const services[]) {
  return handle_services(services, true);
}

static int stop_services(const char *const services[]) {
  return handle_services(services, false);
}

int service_startup(uint64_t watchdog_interval) {
  int r;
  /* connect and authenticate with systemdbus */

  r = sd_bus_open_system(&SDBus);
  if (r < 0) {
    fprintf(stderr, "[service_startup]: Failed to open sd bus: %s\n", strerror(-r));
    return r;
  }

  /* set sd_bus timeout */
  r = sd_bus_set_method_call_timeout(SDBus, watchdog_interval);
  if (r < 0) {
    fprintf(stderr, "[service_startup]: Failed to set sd bus timeout value to watchdog interval: %s\n", strerror(-r));
    return r;
  }

  return EXIT_SUCCESS;
}

int service_shutdown() {
  sd_bus_flush_close_unref(SDBus);
  return EXIT_SUCCESS;
}

int service_reload() {
  /* FILL IN */
  return EXIT_SUCCESS;
}

int service_oneshot() {
  /* FILL IN */
  return EXIT_SUCCESS;
}

int service_loop() {

  /* First determine our current state */

  const State *current_state = NULL;
  int r;

  for (size_t i = 0; i < sizeof(States) / sizeof(State*); i++) {
    if (States[i]->isActive() && (!current_state->StateID || States[i]->Priority > current_state->Priority)) {
      current_state = States[i];
    }
  }

  if (!current_state->StateID) {
    fprintf(stderr, "[service_loop]: Failed to determine current state!\n");
    return -ENOENT;
  }

  /* now we start services based on our current state */

  r = start_services(current_state->ServicesToStart);
  if (r != EXIT_SUCCESS) {
    fprintf(stderr, "[service_startup]: Failed to start all services for state %d: %s\n", current_state->StateID, strerror(-r));
    return r;
  }

  /* and now we stop services based on our current state */

  r = stop_services(current_state->ServicesToStop);
  if (r != EXIT_SUCCESS) {
    fprintf(stderr, "[service_startup]: Failed to stop all services for state %d: %s\n", current_state->StateID, strerror(-r));
    return r;
  }

  return EXIT_SUCCESS;
}


