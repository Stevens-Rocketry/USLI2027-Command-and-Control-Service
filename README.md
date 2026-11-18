# USLI 2027 Service Template

Template for a C systemd service (`Type=notify-reload`) with watchdog support. Implement your logic in `src/service.c`; everything else is shared infrastructure.
 
---

## Creating a service

Replace `xx` with a two-letter code for the service, e.g. `cc` for Command & Control:

| File | Change |
|---|---|
| `src/xxservice.c`, `src/xxservice.h` | Rename; update the `#include` and header guard |
| `Makefile` | `PROG` and `OBJS` |
| `servicename.service` | Rename to `ccservice.service`; set `Description` and `ExecStart` |
| `.gitignore` | Binary name |
 
---

## Lifecycle

| Function | Runs |
|---|---|
| `service_startup()` | Once, before readiness is reported |
| `service_oneshot()` | Once, after readiness is reported |
| `service_loop()` | Repeatedly while running |
| `service_reload()` | On `SIGHUP` (`systemctl reload`) |
| `service_shutdown()` | On `SIGTERM`/`SIGINT`, before exit |

Return `0` on success or a negative errno (e.g. `-EIO`) on failure. A failure exits the process and systemd restarts it after 1 s.

With `WatchdogSec` set, `service_loop()` runs every half watchdog interval and must return within the full interval. Without it, the loop sleeps `SLEEP_TIME` seconds (`src/xxservice.h`).
 
---

## Build

Requires libsystemd headers (`libsystemd-dev` on Debian/Ubuntu, `systemd-libs` on Arch).

```sh
make          # build
make depend   # regenerate header dependencies
make clean
```

Warnings are errors.
 
---

## Run

```sh
sudo install -m 755 ccservice /usr/bin/
sudo install -m 644 ccservice.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable --now ccservice
journalctl -u ccservice -f
```
 
---

## Layout

| Path | Contents |
|---|---|
| `src/service.c`, `src/service.h` | Service hooks |
| `src/xxservice.c`, `src/xxservice.h` | `main()`: signals, systemd notifications, watchdog, loop timing |
| `src/notify.c`, `src/notify.h` | Reload notification for `Type=notify-reload` |
| `servicename.service` | Unit file template |
 