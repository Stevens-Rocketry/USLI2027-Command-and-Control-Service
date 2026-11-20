/*
 *    George K Oliynyk
 *    Stevens Rocketry USLI 2027
 */

#include "sdbus.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include <systemd/sd-bus.h>

#define _cleanup_(f) __attribute__((cleanup(f)))
#define DESTINATION "org.freedesktop.systemd1"
#define PATH        "/org/freedesktop/systemd1"
#define INTERFACE   "org.freedesktop.systemd1.Manager"

int messageSdBus(char* service_name, const char* member, const char* job_path) {
    _cleanup_(sd_bus_flush_close_unrefp) sd_bus *bus = NULL;
    _cleanup_(sd_bus_error_free) sd_bus_error error = SD_BUS_ERROR_NULL;
    _cleanup_(sd_bus_message_unrefp) sd_bus_message *reply = NULL;
    int r;

    r = sd_bus_open_system(&bus);
    if (r < 0) {
        fprintf(stderr, "[sd_bus_open_system]: Failed to open sd bus: %s\n", strerror(r));
        return r;
    }

    r = sd_bus_call_method(bus, DESTINATION, PATH, INTERFACE, member, &error, &reply, "ss", service_name, "replace");
    if (r < 0) {
        fprintf(stderr, "[sd_bus_open_system]: Failed to call method through sd_bus: %s\n", strerror(r));
        return r;
    }

    r = sd_bus_message_read(reply, "o", &job_path);
    if (r < 0) {
        fprintf(stderr, "[sd_bus_open_system]: Failed to read message return from sd_bus: %s\n", strerror(r));
        return r;
    }

    return EXIT_SUCCESS;
}

int startService(char* service_name, const char* job_path) {
    return messageSdBus(service_name, "StartUnit", job_path);
}

int stopService(char* service_name, const char* job_path) {
    return messageSdBus(service_name, "StopUnit", job_path);
}