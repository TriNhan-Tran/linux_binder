# Binder IPC Demo (Module Layout)

This project demonstrates Binder IPC on Linux with four processes:

1. `srv_manager` (context manager, handle `0`)
2. `led_srv`
3. `audio_srv`
4. `app` (client + callback receiver)

## Module-Based Structure

The repository is organized by functionality, not by file type:

```text
app/
  AppCallback.hpp
  AppCallback.cpp
  AppRunner.hpp
  AppRunner.cpp
  main.cpp

core/
  BinderCore.hpp
  BinderCore.cpp
  Parcel.hpp
  Parcel.cpp
  TxnCodes.hpp

driver/
  binder.h
  binder.c

srv/
  AudioSrvIpc.hpp
  AudioSrvIpc.cpp
  LedSrvIpc.hpp
  LedSrvIpc.cpp
  SrvCallbackIpc.hpp
  SrvCallbackIpc.cpp
  SrvCallbackRegistry.hpp
  SrvCallbackRegistry.cpp
  SrvManagerIpc.hpp
  SrvManagerIpc.cpp
  manager/
    SrvManagerHost.hpp
    SrvManagerHost.cpp
    SrvManagerApp.hpp
    SrvManagerApp.cpp
    main.cpp
  led/
    LedSrvHost.hpp
    LedSrvHost.cpp
    LedSrvApp.hpp
    LedSrvApp.cpp
    main.cpp
  audio/
    AudioSrvHost.hpp
    AudioSrvHost.cpp
    AudioSrvApp.hpp
    AudioSrvApp.cpp
    main.cpp
```

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

`run.sh` does not build sources. It only:

1. ensures binderfs is mounted
2. verifies binaries exist in `build/`
3. launches `srv_manager`, `led_srv`, `audio_srv`
4. runs `app` test sequence
5. cleans up background services

## Naming Rules

- `Srv` suffix is used for service/server classes and files
- C driver layer (`driver/*`) remains `snake_case`
- C++ private members use `m_`

## Requirements

- Linux kernel with binder and binderfs enabled
- Binder device path available at `/dev/binderfs/binder`
- CMake `>= 3.16`
- C11/C++17 toolchain
