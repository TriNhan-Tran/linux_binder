# Binder IPC Demo (AOSP-Inspired Layout)

This repository demonstrates a Linux Binder IPC architecture with an
AOSP-style split between binder framework code, service implementations,
and app-side adapters.

Processes in the demo:

1. `srv_manager` (context manager, Binder handle `0`)
2. `led_srv`
3. `audio_srv`
4. `app` (client + callback receiver)

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
  core/
    BinderClient.h
    BinderClient.cpp
    BinderUtils.h
    BinderSrvMgr.h
    BinderSrvMgr.cpp
    Parcel.h
    Parcel.cpp
    TransactionCode.h

  manager/
    ISrvMgr.h
    BnSrvMgr.h
    BnSrvMgr.cpp
    BpSrvMgr.h
    BpSrvMgr.cpp
    SrvMgr.h
    SrvMgr.cpp
    main.cpp

  callback/
    interfaces/
      ISrvCallback.h
      IAudioSrvCallback.h
      ILedSrvCallback.h
    native/
      BnSrvCallback.h
      BnSrvCallback.cpp
      BnAudioSrvCallback.h
      BnAudioSrvCallback.cpp
      BnLedSrvCallback.h
      BnLedSrvCallback.cpp
    proxy/
      BpSrvCallback.h
      BpSrvCallback.cpp
      BpAudioSrvCallback.h
      BpAudioSrvCallback.cpp
      BpLedSrvCallback.h
      BpLedSrvCallback.cpp
    registry/
      SrvCallbackRegistry.h
      SrvCallbackRegistry.cpp

  srv/
    led/
      ILedSrv.h
      BnLedSrv.h
      BnLedSrv.cpp
      BpLedSrv.h
      BpLedSrv.cpp
    audio/
      IAudioSrv.h
      BnAudioSrv.h
      BnAudioSrv.cpp
      BpAudioSrv.h
      BpAudioSrv.cpp

srv/
  led-service/
    LedSrv.h
    LedSrv.cpp
    main.cpp
  audio-service/
    AudioSrv.h
    AudioSrv.cpp
    main.cpp

driver/
  binder.h
  binder.c

build.sh
mount.sh
run.sh
```

## Binder Naming Model

- Interface contract: `I<InterfaceName>`
- Client proxy: `Bp<InterfaceName>`
- Server stub: `Bn<InterfaceName>`

Service-manager roles:

- `BinderSrvMgr`: client helper that talks to Binder handle `0`
- `ISrvMgr`, `BpSrvMgr`, `BnSrvMgr`: Binder contract/proxy/stub
- `SrvMgr`: concrete manager registry implementation

## Build and Run

Build and run are intentionally separate.

### Build

```bash
./build.sh
```

This configures CMake in `build/` and compiles all targets.

### Run Demo

```bash
./run.sh
```

`run.sh` does not build. It:

1. Mounts binderfs if needed via `mount.sh`.
2. Verifies `build/srv_manager`, `build/led_srv`, `build/audio_srv`, and `build/app` exist.
3. Starts `srv_manager`, `led_srv`, and `audio_srv` in background.
4. Runs `app` in foreground.
5. Cleans up all background processes on exit.

## Add a New Service

For a new `FooSrv`, keep IPC and implementation separate:

```text
binder/srv/foo/
  IFooSrv.h
  BnFooSrv.h
  BnFooSrv.cpp
  BpFooSrv.h
  BpFooSrv.cpp

srv/foo-service/
  FooSrv.h
  FooSrv.cpp
  main.cpp
```

Typical steps:

1. Add service name and transaction codes in `binder/core/TransactionCode.h`.
2. Define binder interface in `IFooSrv.h`.
3. Implement server dispatch in `BnFooSrv.*`.
4. Implement client marshalling in `BpFooSrv.*`.
5. Implement service behavior in `srv/foo-service/FooSrv.*`.
6. Register service in `srv/foo-service/main.cpp` through `BinderSrvMgr`.
7. Wire modules and executable in `CMakeLists.txt`.

## Requirements

- Linux kernel with Binder and binderfs support enabled.
- Binder device available at `/dev/binderfs/binder`.
- CMake 3.16+.
- C11/C++17 toolchain.
