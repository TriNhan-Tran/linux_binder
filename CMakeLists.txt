cmake_minimum_required(VERSION 3.16)
project(binder_ipc C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic -O2 -g)

# Binder kernel driver C library
add_library(binder_driver STATIC driver/binder.c)
target_include_directories(binder_driver PUBLIC .)

# Common IPC module (core + callbacks + manager binder proxies/stubs)
add_library(ipc_module STATIC
    binder/core/Parcel.cpp
    binder/core/BinderClient.cpp
    binder/core/BinderSrvMgr.cpp
    binder/callback/native/BnLedSrvCallback.cpp
    binder/callback/proxy/BpLedSrvCallback.cpp
    binder/callback/native/BnAudioSrvCallback.cpp
    binder/callback/proxy/BpAudioSrvCallback.cpp
    binder/callback/native/BnSrvCallback.cpp
    binder/callback/proxy/BpSrvCallback.cpp
    binder/callback/registry/SrvCallbackRegistry.cpp
    binder/manager/BnSrvMgr.cpp
    binder/manager/BpSrvMgr.cpp
)
target_include_directories(ipc_module PUBLIC .)
target_link_libraries(ipc_module PUBLIC binder_driver pthread)

# Binder service IPC interfaces (audio/led I/Bp/Bn)
add_library(srv_ipc_module STATIC
    binder/srv/led/BnLedSrv.cpp
    binder/srv/led/BpLedSrv.cpp
    binder/srv/audio/BnAudioSrv.cpp
    binder/srv/audio/BpAudioSrv.cpp
)
target_include_directories(srv_ipc_module PUBLIC .)
target_link_libraries(srv_ipc_module PUBLIC ipc_module)

# App module implementation
add_library(app_module STATIC
    app/App.cpp
    base/Handler.cpp
    app/adapters/LedSrvAdapter.cpp
    app/adapters/AudioSrvAdapter.cpp
)
target_include_directories(app_module PUBLIC .)
target_link_libraries(app_module PUBLIC srv_ipc_module)

# Service process modules
add_library(srv_manager_module STATIC
    binder/manager/SrvMgr.cpp
)
target_include_directories(srv_manager_module PUBLIC .)
target_link_libraries(srv_manager_module PUBLIC ipc_module)

add_library(led_srv_module STATIC
    srv/led-service/LedSrv.cpp
)
target_include_directories(led_srv_module PUBLIC .)
target_link_libraries(led_srv_module PUBLIC srv_ipc_module)

add_library(audio_srv_module STATIC
    srv/audio-service/AudioSrv.cpp
)
target_include_directories(audio_srv_module PUBLIC .)
target_link_libraries(audio_srv_module PUBLIC srv_ipc_module)

# Executables
add_executable(app app/main.cpp)
target_include_directories(app PRIVATE .)
target_link_libraries(app PRIVATE app_module)

add_executable(srv_manager binder/manager/main.cpp)
target_include_directories(srv_manager PRIVATE .)
target_link_libraries(srv_manager PRIVATE srv_manager_module)

add_executable(led_srv srv/led-service/main.cpp)
target_include_directories(led_srv PRIVATE .)
target_link_libraries(led_srv PRIVATE led_srv_module)

add_executable(audio_srv srv/audio-service/main.cpp)
target_include_directories(audio_srv PRIVATE .)
target_link_libraries(audio_srv PRIVATE audio_srv_module)
