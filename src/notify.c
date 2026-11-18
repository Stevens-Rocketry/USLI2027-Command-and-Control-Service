/*
 *    George K Oliynyk
 *    Stevens Rocketry USLI 2027
 */

#include "notify.h"

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <sys/un.h>
#include <systemd/sd-daemon.h>
#include <time.h>

int notify_reloading(void) {
  /* A buffer with length sufficient to format the maximum UINT64 value. */
  char reload_message[sizeof("RELOADING=1\nMONOTONIC_USEC=18446744073709551615")];
  struct timespec ts;
  uint64_t now;

  /* Notify systemd that we are reloading, including a CLOCK_MONOTONIC timestamp in usec
   * so that the program is compatible with a Type=notify-reload service. */

  if (clock_gettime(CLOCK_MONOTONIC, &ts) < 0)
    return -errno;

  if (ts.tv_sec < 0 || ts.tv_nsec < 0 ||
      (uint64_t) ts.tv_sec > (UINT64_MAX - (ts.tv_nsec / 1000ULL)) / 1000000ULL)
    return -EINVAL;

  now = (uint64_t) ts.tv_sec * 1000000ULL + (uint64_t) ts.tv_nsec / 1000ULL;

  if (snprintf(reload_message, sizeof(reload_message), "RELOADING=1\nMONOTONIC_USEC=%" PRIu64, now) < 0)
    return -EINVAL;

  return sd_notify(0, reload_message);
}
