/*
 *    George K Oliynyk
 *    Stevens Rocketry USLI 2027
 */

#ifndef XXSERVICE_SERVICE_H
#define XXSERVICE_SERVICE_H

/* runs to setup the service, before notifying systemd that we are running */
int service_startup(void);
/* runs before service is shutdown */
int service_shutdown(void);

/* runs when service is asked to reload */
int service_reload(void);

/* runs once at the start of the service lifetime */
int service_oneshot(void);

/* runs continuously while the service is alive */
int service_loop(void);

#endif // XXSERVICE_SERVICE_H
