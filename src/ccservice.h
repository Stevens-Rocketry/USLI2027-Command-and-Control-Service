/*
 *    George K Oliynyk
 *    Stevens Rocketry USLI 2027
 */

#ifndef XXSERVICE_H
#define XXSERVICE_H

/*
 * how long we sleep in between loops if watchdog is disabled. If watchdog is enabled,
 * we sleep half of watchdog's time.
 */
#define SLEEP_TIME 5

/* determines what fraction of the watchdog time we actually sleep for in the loop */
#define WATCH_DOG_FACTOR 4

#endif //XXSERVICE_H
