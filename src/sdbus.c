/*
 *    George K Oliynyk
 *    Stevens Rocketry USLI 2027
 */

#include "sdbus.h"

#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include <systemd/sd-bus.h>

#define DESTINATION "org.freedesktop.systemd1"
#define PATH        "/org/freedesktop/systemd1"
#define INTERFACE   "org.freedesktop.systemd1.Manager"

int messageSdBus() {
    _cleanup_(sd_bus_flush_close_unrefp) sd_bus *bus = NULL;
    _cleanup_(sd_bus_error_free) sd_bus_error error = SD_BUS_ERROR_NULL;
    _cleanup_(sd_bus_message_unrefp) sd_bus_message *reply = NULL;
    int r;

    r = sd_bus_open_system(&bus);
    if (r < 0)
        return log_error(r, "Failed to acquire bus");

    r = sd_bus_call_method(bus, DESTINATION, PATH, INTERFACE, MEMBER, &error, &reply, "u", (unsigned) getpid());
    if (r < 0)
        return log_error(r, MEMBER " call failed");

    const char *ans;
    r = sd_bus_message_read(reply, "o", &ans);
    if (r < 0)
        return log_error(r, "Failed to read reply");

    printf("Unit path is \"%s\".\n", ans);

    return 0;
}

int startService(char* ServiceName) {
    int result = 0;
    return 0;
}

int stopService(char* ServiceName) {
    return 0;
}