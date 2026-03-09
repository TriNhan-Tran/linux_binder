# Binder IPC Demo (Service-Centric)

This repository demonstrates a Binder IPC architecture with clear service
boundaries and extension points.

Processes:

1. `srv_manager` (context manager, handle `0`)
2. `led_srv`
3. `audio_srv`
4. `app` (demo client + callback receiver)

## Directory Structure

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

core/
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
  manager/
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
```

## Binder Role Summary

- `I*Srv`: public Binder-facing service contract.
- `Bn*Srv`: server-side Binder stub. It parses incoming transactions and routes
  calls to the real implementation.
- `Bp*Srv`: client-side Binder proxy. It marshals method calls into Binder IPC.
- `*Srv`: real service implementation where behavior lives.
- `*SrvAdapter`: app-side service adapter. It owns service-specific callback
  translation and forwards high-level events to `App`.

## App Callback Architecture

The app no longer mixes all service callbacks in a single `onEvent()` function.

Instead, callback handling is split by service:

- `LedSrvAdapter` handles LED callback transactions only.
- `AudioSrvAdapter` handles audio callback transactions only.
- `App` owns one adapter per service and only receives high-level forwarded
  events.

### Adapter Design

`LedSrvAdapter`:

- Inherits `ipc::BnLedSrvCallback`
- Singleton accessed via `LedSrvAdapter::getInstance()`
- Owns LED client proxy (`ipc::BpLedSrv`) internally
- Receives LED callback events from Binder
- Calls `App::handleLedEvent(...)`

`AudioSrvAdapter`:

- Inherits `ipc::BnAudioSrvCallback`
- Singleton accessed via `AudioSrvAdapter::getInstance()`
- Owns audio client proxy (`ipc::BpAudioSrv`) internally
- Receives audio callback events from Binder
- Calls `App::handleAudioEvent(...)`

### App Usage Pattern

`App` initializes and uses one singleton adapter per service:

1. Get singleton adapters (`LedSrvAdapter::getInstance()`, `AudioSrvAdapter::getInstance()`)
2. Lookup service handles via `ipc::ServiceManager`
3. Initialize adapters with service handles (adapters create `Bp*` internally)
4. Register each adapter callback with corresponding service
5. Handle forwarded events in `handleLedEvent(...)` / `handleAudioEvent(...)`

App never constructs `BpLedSrv`/`BpAudioSrv` directly.

Threading rule:

- Adapters do not create or manage threads.
- Binder callback processing runs on the existing App binder listener context.

## Build and Run

Build and run are intentionally separated.

### Build

```bash
./build.sh
```

### Run Tests

```bash
./run.sh
```

`run.sh` does not compile code. It only:

1. mounts binderfs if needed
2. checks prebuilt binaries in `build/`
3. starts `srv_manager`, `led_srv`, `audio_srv`
4. runs `app`
5. cleans up processes

## Add a New Service

To add a new service `FooSrv`, follow this pattern:

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

1. Define transaction codes and service name in `core/TransactionCode.h`.
2. Define contract in `IFooSrv.h`.
3. Implement Binder stub dispatch in `BnFooSrv.*`.
4. Implement Binder proxy marshalling in `BpFooSrv.*`.
5. Implement real behavior in `FooSrv.*`.
6. Register service from `FooSrvApp.cpp` via `ipc::ServiceManager`.
7. Add sources and executable target in `CMakeLists.txt`.
8. Optionally add a client app in `app/<app_name>/` that depends on `IFooSrv`.

## Add a New Service Adapter

When App starts using a new service, add a matching adapter:

1. Add callback contract in `core/callback`:
  for example `IBarSrvCallback`, `BnBarSrvCallback`, `BpBarSrvCallback`.
2. Add adapter in `app/adapters`:
  for example `BarSrvAdapter` inheriting `ipc::BnBarSrvCallback`.
3. Add one App-level handler method:
  for example `App::handleBarEvent(...)`.
4. Add one adapter member in `App` and instantiate it during `run()`.
5. Register adapter with `BpBarSrv::registerCallback(...)`.
6. Update `CMakeLists.txt` with new callback/adaptor source files.
7. Run `./build.sh` and `./run.sh` to verify callback flow and behavior parity.

## Requirements

- Linux kernel with binder and binderfs enabled
- Binder device path: `/dev/binderfs/binder`
- CMake >= 3.16
- C11/C++17 toolchain
