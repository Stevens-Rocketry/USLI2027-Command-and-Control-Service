/*
 *    George K Oliynyk
 *    Stevens Rocketry USLI 2027
 */

#ifndef SDBUS_H
#define SDBUS_H

/* DEFINES */

/* FUNCTIONS */

int startService(char* service_name, const char* job_path);
int stopService(char* service_name, const char* job_path);

#endif //SDBUS_H
