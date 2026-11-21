/*
 *    George K Oliynyk
 *    Stevens Rocketry USLI 2027
 */

#ifndef SDBUS_H
#define SDBUS_H

/* DEFINES */

/* INCLUDES */

#include <systemd/sd-bus-protocol.h>

/* FUNCTIONS */

int startService(const char* service_name, sd_bus *bus);
int stopService(const char* service_name, sd_bus *bus);
int serviceStatus(const char* service_name, sd_bus *bus, char** state);
int serviceJobID (const char* service_name, sd_bus *bus, uint32_t *job_id);

#endif //SDBUS_H
