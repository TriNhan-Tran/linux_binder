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
.
├── CMakeLists.txt
├── Makefile
├── README.md
├── app
│   ├── App.cpp
│   ├── App.h
│   ├── adapters
│   │   ├── AudioSrvAdapter.cpp
│   │   ├── AudioSrvAdapter.h
│   │   ├── LedSrvAdapter.cpp
│   │   └── LedSrvAdapter.h
│   └── main.cpp
├── base
│   ├── Handler.cpp
│   ├── Handler.h
│   └── MessageQueue.h
├── binder
│   ├── callback
│   │   ├── interfaces
│   │   │   ├── IAudioSrvCallback.h
│   │   │   ├── ILedSrvCallback.h
│   │   │   └── ISrvCallback.h
│   │   ├── native
│   │   │   ├── BnAudioSrvCallback.cpp
│   │   │   ├── BnAudioSrvCallback.h
│   │   │   ├── BnLedSrvCallback.cpp
│   │   │   ├── BnLedSrvCallback.h
│   │   │   ├── BnSrvCallback.cpp
│   │   │   └── BnSrvCallback.h
│   │   ├── proxy
│   │   │   ├── BpAudioSrvCallback.cpp
│   │   │   ├── BpAudioSrvCallback.h
│   │   │   ├── BpLedSrvCallback.cpp
│   │   │   ├── BpLedSrvCallback.h
│   │   │   ├── BpSrvCallback.cpp
│   │   │   └── BpSrvCallback.h
│   │   └── registry
│   │       ├── SrvCallbackRegistry.cpp
│   │       └── SrvCallbackRegistry.h
│   ├── core
│   │   ├── BinderClient.cpp
│   │   ├── BinderClient.h
│   │   ├── BinderSrvMgr.cpp
│   │   ├── BinderSrvMgr.h
│   │   ├── Parcel.cpp
│   │   ├── Parcel.h
│   │   └── TransactionCode.h
│   ├── manager
│   │   ├── BnSrvMgr.cpp
│   │   ├── BnSrvMgr.h
│   │   ├── BpSrvMgr.cpp
│   │   ├── BpSrvMgr.h
│   │   ├── ISrvMgr.h
│   │   ├── SrvMgr.cpp
│   │   ├── SrvMgr.h
│   │   └── main.cpp
│   └── srv
│       ├── audio
│       │   ├── BnAudioSrv.cpp
│       │   ├── BnAudioSrv.h
│       │   ├── BpAudioSrv.cpp
│       │   ├── BpAudioSrv.h
│       │   └── IAudioSrv.h
│       └── led
│           ├── BnLedSrv.cpp
│           ├── BnLedSrv.h
│           ├── BpLedSrv.cpp
│           ├── BpLedSrv.h
│           └── ILedSrv.h
├── build
│   ├── CMakeCache.txt
│   ├── CMakeFiles
│   │   ├── 3.28.1
│   │   │   ├── CMakeCCompiler.cmake
│   │   │   ├── CMakeCXXCompiler.cmake
│   │   │   ├── CMakeDetermineCompilerABI_C.bin
│   │   │   ├── CMakeDetermineCompilerABI_CXX.bin
│   │   │   ├── CMakeSystem.cmake
│   │   │   ├── CompilerIdC
│   │   │   │   ├── CMakeCCompilerId.c
│   │   │   │   ├── a.out
│   │   │   │   └── tmp
│   │   │   └── CompilerIdCXX
│   │   │       ├── CMakeCXXCompilerId.cpp
│   │   │       ├── a.out
│   │   │       └── tmp
│   │   ├── CMakeConfigureLog.yaml
│   │   ├── CMakeDirectoryInformation.cmake
│   │   ├── Makefile.cmake
│   │   ├── Makefile2
│   │   ├── TargetDirectories.txt
│   │   ├── app.dir
│   │   │   ├── DependInfo.cmake
│   │   │   ├── app
│   │   │   │   ├── main.cpp.o
│   │   │   │   └── main.cpp.o.d
│   │   │   ├── build.make
│   │   │   ├── cmake_clean.cmake
│   │   │   ├── compiler_depend.internal
│   │   │   ├── compiler_depend.make
│   │   │   ├── compiler_depend.ts
│   │   │   ├── depend.make
│   │   │   ├── flags.make
│   │   │   ├── link.txt
│   │   │   └── progress.make
│   │   ├── app_module.dir
│   │   │   ├── DependInfo.cmake
│   │   │   ├── app
│   │   │   │   ├── App.cpp.o
│   │   │   │   ├── App.cpp.o.d
│   │   │   │   └── adapters
│   │   │   │       ├── AudioSrvAdapter.cpp.o
│   │   │   │       ├── AudioSrvAdapter.cpp.o.d
│   │   │   │       ├── LedSrvAdapter.cpp.o
│   │   │   │       └── LedSrvAdapter.cpp.o.d
│   │   │   ├── base
│   │   │   │   ├── Handler.cpp.o
│   │   │   │   └── Handler.cpp.o.d
│   │   │   ├── build.make
│   │   │   ├── cmake_clean.cmake
│   │   │   ├── cmake_clean_target.cmake
│   │   │   ├── compiler_depend.internal
│   │   │   ├── compiler_depend.make
│   │   │   ├── compiler_depend.ts
│   │   │   ├── depend.make
│   │   │   ├── flags.make
│   │   │   ├── link.txt
│   │   │   └── progress.make
│   │   ├── audio_srv.dir
│   │   │   ├── DependInfo.cmake
│   │   │   ├── build.make
│   │   │   ├── cmake_clean.cmake
│   │   │   ├── compiler_depend.internal
│   │   │   ├── compiler_depend.make
│   │   │   ├── compiler_depend.ts
│   │   │   ├── depend.make
│   │   │   ├── flags.make
│   │   │   ├── link.txt
│   │   │   ├── progress.make
│   │   │   └── srv
│   │   │       └── audio-service
│   │   │           ├── main.cpp.o
│   │   │           └── main.cpp.o.d
│   │   ├── audio_srv_module.dir
│   │   │   ├── DependInfo.cmake
│   │   │   ├── build.make
│   │   │   ├── cmake_clean.cmake
│   │   │   ├── cmake_clean_target.cmake
│   │   │   ├── compiler_depend.internal
│   │   │   ├── compiler_depend.make
│   │   │   ├── compiler_depend.ts
│   │   │   ├── depend.make
│   │   │   ├── flags.make
│   │   │   ├── link.txt
│   │   │   ├── progress.make
│   │   │   └── srv
│   │   │       └── audio-service
│   │   │           ├── AudioSrv.cpp.o
│   │   │           └── AudioSrv.cpp.o.d
│   │   ├── binder_driver.dir
│   │   │   ├── DependInfo.cmake
│   │   │   ├── build.make
│   │   │   ├── cmake_clean.cmake
│   │   │   ├── cmake_clean_target.cmake
│   │   │   ├── compiler_depend.internal
│   │   │   ├── compiler_depend.make
│   │   │   ├── compiler_depend.ts
│   │   │   ├── depend.make
│   │   │   ├── driver
│   │   │   │   ├── binder.c.o
│   │   │   │   └── binder.c.o.d
│   │   │   ├── flags.make
│   │   │   ├── link.txt
│   │   │   └── progress.make
│   │   ├── cmake.check_cache
│   │   ├── ipc_module.dir
│   │   │   ├── DependInfo.cmake
│   │   │   ├── binder
│   │   │   │   ├── callback
│   │   │   │   │   ├── native
│   │   │   │   │   │   ├── BnAudioSrvCallback.cpp.o
│   │   │   │   │   │   ├── BnAudioSrvCallback.cpp.o.d
│   │   │   │   │   │   ├── BnLedSrvCallback.cpp.o
│   │   │   │   │   │   ├── BnLedSrvCallback.cpp.o.d
│   │   │   │   │   │   ├── BnSrvCallback.cpp.o
│   │   │   │   │   │   └── BnSrvCallback.cpp.o.d
│   │   │   │   │   ├── proxy
│   │   │   │   │   │   ├── BpAudioSrvCallback.cpp.o
│   │   │   │   │   │   ├── BpAudioSrvCallback.cpp.o.d
│   │   │   │   │   │   ├── BpLedSrvCallback.cpp.o
│   │   │   │   │   │   ├── BpLedSrvCallback.cpp.o.d
│   │   │   │   │   │   ├── BpSrvCallback.cpp.o
│   │   │   │   │   │   └── BpSrvCallback.cpp.o.d
│   │   │   │   │   └── registry
│   │   │   │   │       ├── SrvCallbackRegistry.cpp.o
│   │   │   │   │       └── SrvCallbackRegistry.cpp.o.d
│   │   │   │   ├── core
│   │   │   │   │   ├── BinderClient.cpp.o
│   │   │   │   │   ├── BinderClient.cpp.o.d
│   │   │   │   │   ├── BinderSrvMgr.cpp.o
│   │   │   │   │   ├── BinderSrvMgr.cpp.o.d
│   │   │   │   │   ├── Parcel.cpp.o
│   │   │   │   │   └── Parcel.cpp.o.d
│   │   │   │   └── manager
│   │   │   │       ├── BnSrvMgr.cpp.o
│   │   │   │       ├── BnSrvMgr.cpp.o.d
│   │   │   │       ├── BpSrvMgr.cpp.o
│   │   │   │       └── BpSrvMgr.cpp.o.d
│   │   │   ├── build.make
│   │   │   ├── cmake_clean.cmake
│   │   │   ├── cmake_clean_target.cmake
│   │   │   ├── compiler_depend.internal
│   │   │   ├── compiler_depend.make
│   │   │   ├── compiler_depend.ts
│   │   │   ├── depend.make
│   │   │   ├── flags.make
│   │   │   ├── link.txt
│   │   │   └── progress.make
│   │   ├── led_srv.dir
│   │   │   ├── DependInfo.cmake
│   │   │   ├── build.make
│   │   │   ├── cmake_clean.cmake
│   │   │   ├── compiler_depend.internal
│   │   │   ├── compiler_depend.make
│   │   │   ├── compiler_depend.ts
│   │   │   ├── depend.make
│   │   │   ├── flags.make
│   │   │   ├── link.txt
│   │   │   ├── progress.make
│   │   │   └── srv
│   │   │       └── led-service
│   │   │           ├── main.cpp.o
│   │   │           └── main.cpp.o.d
│   │   ├── led_srv_module.dir
│   │   │   ├── DependInfo.cmake
│   │   │   ├── build.make
│   │   │   ├── cmake_clean.cmake
│   │   │   ├── cmake_clean_target.cmake
│   │   │   ├── compiler_depend.internal
│   │   │   ├── compiler_depend.make
│   │   │   ├── compiler_depend.ts
│   │   │   ├── depend.make
│   │   │   ├── flags.make
│   │   │   ├── link.txt
│   │   │   ├── progress.make
│   │   │   └── srv
│   │   │       └── led-service
│   │   │           ├── LedSrv.cpp.o
│   │   │           └── LedSrv.cpp.o.d
│   │   ├── pkgRedirects
│   │   ├── progress.marks
│   │   ├── srv_ipc_module.dir
│   │   │   ├── DependInfo.cmake
│   │   │   ├── binder
│   │   │   │   └── srv
│   │   │   │       ├── audio
│   │   │   │       │   ├── BnAudioSrv.cpp.o
│   │   │   │       │   ├── BnAudioSrv.cpp.o.d
│   │   │   │       │   ├── BpAudioSrv.cpp.o
│   │   │   │       │   └── BpAudioSrv.cpp.o.d
│   │   │   │       └── led
│   │   │   │           ├── BnLedSrv.cpp.o
│   │   │   │           ├── BnLedSrv.cpp.o.d
│   │   │   │           ├── BpLedSrv.cpp.o
│   │   │   │           └── BpLedSrv.cpp.o.d
│   │   │   ├── build.make
│   │   │   ├── cmake_clean.cmake
│   │   │   ├── cmake_clean_target.cmake
│   │   │   ├── compiler_depend.internal
│   │   │   ├── compiler_depend.make
│   │   │   ├── compiler_depend.ts
│   │   │   ├── depend.make
│   │   │   ├── flags.make
│   │   │   ├── link.txt
│   │   │   └── progress.make
│   │   ├── srv_manager.dir
│   │   │   ├── DependInfo.cmake
│   │   │   ├── binder
│   │   │   │   └── manager
│   │   │   │       ├── main.cpp.o
│   │   │   │       └── main.cpp.o.d
│   │   │   ├── build.make
│   │   │   ├── cmake_clean.cmake
│   │   │   ├── compiler_depend.internal
│   │   │   ├── compiler_depend.make
│   │   │   ├── compiler_depend.ts
│   │   │   ├── depend.make
│   │   │   ├── flags.make
│   │   │   ├── link.txt
│   │   │   └── progress.make
│   │   └── srv_manager_module.dir
│   │       ├── DependInfo.cmake
│   │       ├── binder
│   │       │   └── manager
│   │       │       ├── SrvMgr.cpp.o
│   │       │       └── SrvMgr.cpp.o.d
│   │       ├── build.make
│   │       ├── cmake_clean.cmake
│   │       ├── cmake_clean_target.cmake
│   │       ├── compiler_depend.internal
│   │       ├── compiler_depend.make
│   │       ├── compiler_depend.ts
│   │       ├── depend.make
│   │       ├── flags.make
│   │       ├── link.txt
│   │       └── progress.make
│   ├── Makefile
│   ├── app
│   ├── audio_srv
│   ├── cmake_install.cmake
│   ├── led_srv
│   ├── libapp_module.a
│   ├── libaudio_srv_module.a
│   ├── libbinder_driver.a
│   ├── libipc_module.a
│   ├── libled_srv_module.a
│   ├── libsrv_ipc_module.a
│   ├── libsrv_manager_module.a
│   └── srv_manager
├── build.sh
├── driver
│   ├── binder.c
│   └── binder.h
├── mount.sh
├── run.sh
└── srv
    ├── audio-service
    │   ├── AudioSrv.cpp
    │   ├── AudioSrv.h
    │   └── main.cpp
    └── led-service
        ├── LedSrv.cpp
        ├── LedSrv.h
        └── main.cpp
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
