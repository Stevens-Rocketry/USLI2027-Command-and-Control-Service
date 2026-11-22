/*
 *    George K Oliynyk
 *    Stevens Rocketry USLI 2027
 */
#include "ccservice.h"

#include <errno.h>
#include <inttypes.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <systemd/sd-daemon.h>

#include "notify.h"
#include "service.h"

static volatile sig_atomic_t reloading = 0;
static volatile sig_atomic_t terminating = 0;

static void signal_handler(int sig) {
  if (sig == SIGHUP)
    reloading = 1;
  else if (sig == SIGINT || sig == SIGTERM)
    terminating = 1;
}

int main(void) {
  struct sigaction sa = {
    .sa_handler = signal_handler,
    .sa_flags = SA_RESTART,
  };
  int r;

  /* Setup signal handlers */
  sigemptyset(&sa.sa_mask);
  sigaction(SIGHUP, &sa, NULL);
  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGTERM, &sa, NULL);

  /* watchdog setup */
  uint64_t watchdog_interval;
  int watchdog_enabled;

  watchdog_enabled = sd_watchdog_enabled(0, &watchdog_interval);

  if (watchdog_enabled) {
    watchdog_interval /= 2;
  }

  /* run custom service startup */

  r = service_startup(watchdog_interval);
  if (r < 0) {
    fprintf(stderr, "Failed to complete startup: %s\n", strerror(-r));
    return EXIT_FAILURE;
  }

  r = sd_notify(0, "READY=1");
  if (r < 0) {
    fprintf(stderr, "Failed to notify readiness to $NOTIFY_SOCKET: %s\n", strerror(-r));
    return EXIT_FAILURE;
  }

  r = service_oneshot();
  if (r < 0) {
    fprintf(stderr, "Failed to execute one shot function: %s\n", strerror(-r));
    return EXIT_FAILURE;
  }

  while (!terminating) {
    if (reloading) {
      reloading = false;

      r = notify_reloading();
      if (r < 0) {
        fprintf(stderr, "Failed to notify reloading to $NOTIFY_SOCKET: %s\n", strerror(-r));
        return EXIT_FAILURE;
      }

      r = service_reload();
      if (r < 0) {
        fprintf(stderr, "Failed to complete service reload: %s\n", strerror(-r));
        return EXIT_FAILURE;
      }

      r = sd_notify(0, "READY=1");
      if (r < 0) {
        fprintf(stderr, "Failed to notify readiness to $NOTIFY_SOCKET: %s\n", strerror(-r));
        return EXIT_FAILURE;
      }
    }

    r = service_loop();
    if (r < 0) {
      fprintf(stderr, "Failed to service loop iteration: %s\n", strerror(-r));
      return EXIT_FAILURE;
    }

    if (watchdog_enabled) {
      r = sd_notify(0, "WATCHDOG=1");
      usleep(watchdog_interval);
    } else {
      sleep(SLEEP_TIME);
    }
  }

  r = sd_notify(0, "STOPPING=1");
  if (r < 0) {
    fprintf(stderr, "Failed to report termination to $NOTIFY_SOCKET: %s\n", strerror(-r));
    return EXIT_FAILURE;
  }

  r = service_shutdown();
  if (r < 0) {
    fprintf(stderr, "Failed to execute service shutdown: %s\n", strerror(-r));
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}