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
#define M_INTERFACE "org.freedesktop.systemd1.Manager"
#define U_INTERFACE "org.freedesktop.systemd1.Unit"

static inline void freep(void *p) { free(*(void **)p); }

int messageSdBus(const char* service_name, const char* member, sd_bus *bus) {
    _cleanup_(sd_bus_error_free) sd_bus_error error = SD_BUS_ERROR_NULL;
    _cleanup_(sd_bus_message_unrefp) sd_bus_message *reply = NULL;

    const char* job_path;
    int r;

    r = sd_bus_call_method(bus, DESTINATION, PATH, M_INTERFACE, member, &error, &reply, "ss", service_name, "replace");
    if (r < 0) {
        fprintf(stderr, "[sd_bus]: Failed to call method through sd_bus: %s\n", error.message ? error.message : strerror(-r));
        return r;
    }

    r = sd_bus_message_read(reply, "o", &job_path);
    if (r < 0) {
        fprintf(stderr, "[sd_bus]: Failed to read message return from sd_bus: %s\n", strerror(-r));
        return r;
    }

    return EXIT_SUCCESS;
}

int startService(const char* service_name, sd_bus *bus) {
    return messageSdBus(service_name, "StartUnit", bus);
}

int stopService(const char* service_name, sd_bus *bus) {
    return messageSdBus(service_name, "StopUnit", bus);
}

int serviceStatus(const char* service_name, sd_bus *bus, char** state) {
    _cleanup_(sd_bus_error_free) sd_bus_error error = SD_BUS_ERROR_NULL;
    _cleanup_(freep) char *unit_path = NULL;

    int r;

    r = sd_bus_path_encode("/org/freedesktop/systemd1/unit", service_name, &unit_path);
    if (r < 0) {
        fprintf(stderr, "[sd_bus]: Failed to encode path for %s: %s\n", service_name, strerror(-r));
        return r;
    }

    r = sd_bus_get_property_string(bus, DESTINATION, unit_path,U_INTERFACE, "ActiveState", &error, state);

    if (r < 0) {
        fprintf(stderr, "[sd_bus]: Failed to get unit property: %s\n", error.message ? error.message : strerror(-r));
        return r;
    }

    return EXIT_SUCCESS;
}

int serviceJobID (const char* service_name, sd_bus *bus, uint32_t *job_id) {
    _cleanup_(sd_bus_error_free) sd_bus_error error = SD_BUS_ERROR_NULL;
    _cleanup_(sd_bus_message_unrefp) sd_bus_message *reply = NULL;
    _cleanup_(freep) char *unit_path = NULL;

    const char* job_path;
    int r;

    r = sd_bus_path_encode("/org/freedesktop/systemd1/unit", service_name, &unit_path);
    if (r < 0) {
        fprintf(stderr, "[sd_bus]: Failed to encode path for %s: %s\n", service_name, strerror(-r));
        return r;
    }

    r = sd_bus_get_property(bus, "org.freedesktop.systemd1", unit_path,"org.freedesktop.systemd1.Unit", "Job", &error, &reply, "(uo)");
    if (r < 0) {
        fprintf(stderr, "[sd_bus]: Failed to get unit job ID: %s\n", error.message ? error.message : strerror(-r));
        return r;
    }

    r = sd_bus_message_read(reply, "(uo)", job_id, &job_path);
    if (r < 0) {
        fprintf(stderr, "[sd_bus]: Failed to read job_id from sd_message: %s\n", strerror(-r));
        return r;
    }

    return EXIT_SUCCESS;
}