# USLI2027-Command-and-Control-Service

Command & control systemd service (`ccservice`) that starts and stops payload services based on the current flight state.

## Requirements

- libsystemd (Arch: `systemd-libs`, Debian/Ubuntu: `libsystemd-dev`), and systemd 253 or newer at runtime
- gcc with C23 support, make
- Root access, or some other way to start and stop units on the system bus

## Build

```sh
make
```

## Install and run

```sh
sudo install -m 0755 ccservice /usr/bin/ccservice
sudo install -m 0644 commandandcontrol.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable --now commandandcontrol
```

On the payload image, the `ccservice` recipe in [RocketryUSLIPayloadYocto20262027](https://github.com/Stevens-Rocketry/RocketryUSLIPayloadYocto20262027) installs and enables it.

## Test

With [USLI2027-Test-Service](https://github.com/Stevens-Rocketry/USLI2027-Test-Service) installed, the test state should start it within one loop:

```sh
systemctl is-active test           # should print "active"
systemctl reload commandandcontrol # should log a reload and return to active
journalctl -u commandandcontrol -f # view logs
```

---

## How it works

On each loop, `ccservice` polls every state's `isActive()`. Of the active states, the one with the highest `Priority` wins. The service then starts each unit in that state's `ServicesToStart` and stops each unit in its `ServicesToStop`. Units in neither list are left alone.

Managed units must be installed disabled and define their own `Restart=` policy.

## Adding a state

1. Copy `src/states/test.{c,h}` to `src/states/<name>.{c,h}`.
2. Set a unique `ID_` and a `PRIORITY_`. Keep `isActive()` static and non-blocking.
3. List its units in `ServicesToStart` and `ServicesToStop`, at most `MAXSERVICES` (5) per list.
4. Include the header in `src/service.c` and add `&<Name>State` to `States[]`.
5. Add `<name>.o` to `OBJS` in the `Makefile`.

## Layout

| What | Where |
|---|---|
| Daemon lifecycle (signals, watchdog, sd_notify) | `src/ccservice.c` |
| State selection and unit control | `src/service.c` |
| systemd D-Bus helpers | `src/sdbus.c` |
| Timing constants | `src/ccservice.h` |