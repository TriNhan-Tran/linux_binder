# Binder IPC Demo (Service-Centric)

This repository demonstrates a Linux Binder IPC architecture with clear service
boundaries, app-side adapters, and a reusable lifecycle base class.

Processes:

1. `srv_manager` (context manager, Binder handle `0`)
2. `led_srv`
3. `audio_srv`
4. `app` (demo client + callback receiver)

## Repository Layout

```text
app/
  App.h
  App.cpp
  main.cpp
  adapters/
    LedSrvAdapter.h
    LedSrvAdapter.cpp
    AudioSrvAdapter.h
    AudioSrvAdapter.cpp

base/
  Handler.h
  Handler.cpp
  MessageQueue.h

binder/
  BinderClient.h
  BinderClient.cpp
  BinderUtils.h
  Parcel.h
  Parcel.cpp
  ServiceManager.h
  ServiceManager.cpp
  TransactionCode.h
  callback/
    ILedSrvCallback.h
    BnLedSrvCallback.h
    BnLedSrvCallback.cpp
    BpLedSrvCallback.h
    BpLedSrvCallback.cpp
    IAudioSrvCallback.h
    BnAudioSrvCallback.h
    BnAudioSrvCallback.cpp
    BpAudioSrvCallback.h
    BpAudioSrvCallback.cpp
    ISrvCallback.h
    BnSrvCallback.h
    BnSrvCallback.cpp
    BpSrvCallback.h
    BpSrvCallback.cpp
    SrvCallbackRegistry.h
    SrvCallbackRegistry.cpp
  srv/
    ISrvManager.h
    BnSrvManager.h
    BnSrvManager.cpp
    BpSrvManager.h
    BpSrvManager.cpp
    SrvManager.h
    SrvManager.cpp
    SrvManagerApp.h
    SrvManagerApp.cpp
    main.cpp

driver/
  binder.h
  binder.c

srv/
  led/
    ILedSrv.h
    BnLedSrv.h
    BnLedSrv.cpp
    BpLedSrv.h
    BpLedSrv.cpp
    LedSrv.h
    LedSrv.cpp
    LedSrvApp.h
    LedSrvApp.cpp
    main.cpp
  audio/
    IAudioSrv.h
    BnAudioSrv.h
    BnAudioSrv.cpp
    BpAudioSrv.h
    BpAudioSrv.cpp
    AudioSrv.h
    AudioSrv.cpp
    AudioSrvApp.h
    AudioSrvApp.cpp
    main.cpp

build.sh
run.sh
mount.sh
```

## Binder Role Summary

- `I*Srv`: public Binder-facing service contract.
- `Bn*Srv`: server-side Binder stub. Parses incoming transactions and routes
  calls to the implementation.
- `Bp*Srv`: client-side Binder proxy. Marshals API calls to Binder IPC.
- `*Srv`: concrete service behavior.
- `*SrvAdapter`: app-side callback adapter that translates Binder callbacks into
  app-level events.

## App Architecture

`App` does not directly construct service proxies. It uses one adapter per
service:

- `LedSrvAdapter` for LED service callbacks and requests.
- `AudioSrvAdapter` for audio service callbacks and requests.

Adapters forward callback payloads into the app message pipeline by posting
`Message` objects to `Handler::sendMessage(...)`. `App::handleMessage(...)`
dispatches to:

- `handleLedEvent(...)`
- `handleAudioEvent(...)`

This keeps Binder callback handling separated from app business flow.

## Lifecycle Model (NVI)

`base/Handler` exposes a non-virtual lifecycle API:

- `start()`
- `run()`
- `stop()`

And protected hooks for subclasses:

- `onInit()`
- `onStart()`
- `onRun()`
- `onStop()`
- `handleMessage(...)`

`app/main.cpp` drives the demo in this order:

```cpp
demo::App app;
app.start();
app.run();
app.stop();
```

Shutdown behavior is graceful:

- App keeps the Binder pool thread joinable.
- `onStop()` closes `ProcessState`, joins the Binder thread, and releases
  service resources.
- Driver loop treats `EBADF`/`ENODEV` as normal shutdown paths (no noisy stop
  log for expected fd close).

## Build and Run

Build and run are intentionally separate.

### Build

```bash
./build.sh
```

### Run Demo

```bash
./run.sh
```

`run.sh` does not build. It:

1. Mounts binderfs if needed (`mount.sh`).
2. Verifies binaries exist in `build/`.
3. Starts `srv_manager`, `led_srv`, and `audio_srv`.
4. Runs `app` in the foreground.
5. Cleans up background processes.

## Add a New Service

For a new `FooSrv`, follow this shape:

```text
srv/foo/
  IFooSrv.h
  BnFooSrv.h
  BnFooSrv.cpp
  BpFooSrv.h
  BpFooSrv.cpp
  FooSrv.h
  FooSrv.cpp
  FooSrvApp.h
  FooSrvApp.cpp
  main.cpp
```

Steps:

1. Add service name and transaction codes in `binder/TransactionCode.h`.
2. Define Binder contract in `IFooSrv.h`.
3. Implement stub dispatch in `BnFooSrv.*`.
4. Implement proxy marshalling in `BpFooSrv.*`.
5. Implement behavior in `FooSrv.*`.
6. Register service in `FooSrvApp.cpp` through `ipc::ServiceManager`.
7. Add library/executable wiring in `CMakeLists.txt`.

## Add a New App Adapter

When `App` starts using a new service, add a matching adapter:

1. Add callback contract types under `binder/callback`.
2. Add adapter implementation under `app/adapters`.
3. Add event payload + message ID in `App`.
4. Initialize/register adapter in `onInit()`/`onStart()`.
5. Handle forwarded message in `App::handleMessage(...)`.
6. Add new files to `CMakeLists.txt` if needed.
7. Verify with `./build.sh` and `./run.sh`.

## Requirements

- Linux kernel with Binder and binderfs enabled.
- Binder device path available at `/dev/binderfs/binder`.
- CMake 3.16+.
- C11/C++17 toolchain.
