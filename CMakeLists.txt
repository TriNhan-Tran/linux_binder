cmake_minimum_required(VERSION 3.16)
project(binder_ipc C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic -O2 -g)

# Binder kernel driver C library
add_library(binder_driver STATIC driver/binder.c)
target_include_directories(binder_driver PUBLIC driver)

# Common IPC module
add_library(ipc_module STATIC
    core/Parcel.cpp
    core/BinderClient.cpp
    core/ServiceManager.cpp
    core/callback/BnLedSrvCallback.cpp
    core/callback/BpLedSrvCallback.cpp
    core/callback/BnAudioSrvCallback.cpp
    core/callback/BpAudioSrvCallback.cpp
    core/callback/BnSrvCallback.cpp
    core/callback/BpSrvCallback.cpp
    core/callback/SrvCallbackRegistry.cpp
    core/manager/BnSrvManager.cpp
    core/manager/BpSrvManager.cpp
)
target_include_directories(ipc_module PUBLIC
    core
    core/callback
    core/manager
    driver
)
target_link_libraries(ipc_module PUBLIC binder_driver pthread)

# Binder interface/stub/proxy module
add_library(srv_ipc_module STATIC
    srv/led/BnLedSrv.cpp
    srv/led/BpLedSrv.cpp
    srv/audio/BnAudioSrv.cpp
    srv/audio/BpAudioSrv.cpp
)
target_include_directories(srv_ipc_module PUBLIC
    core
    core/callback
    core/manager
    srv
    srv/led
    srv/audio
    driver
)
target_link_libraries(srv_ipc_module PUBLIC ipc_module)

# App module implementation
add_library(app_module STATIC
    app/App.cpp
    app/adapters/LedSrvAdapter.cpp
    app/adapters/AudioSrvAdapter.cpp
)
target_include_directories(app_module PUBLIC
    app
    app/adapters
    core
    core/callback
    core/manager
    srv/led
    srv/audio
    driver
)
target_link_libraries(app_module PUBLIC srv_ipc_module)

# Service modules
add_library(srv_manager_module STATIC
    core/manager/SrvManager.cpp
    core/manager/SrvManagerApp.cpp
)
target_include_directories(srv_manager_module PUBLIC
    core
    core/manager
    driver
)
target_link_libraries(srv_manager_module PUBLIC ipc_module)

add_library(led_srv_module STATIC
    srv/led/LedSrv.cpp
    srv/led/LedSrvApp.cpp
)
target_include_directories(led_srv_module PUBLIC
    srv/led
    core
    core/callback
    driver
)
target_link_libraries(led_srv_module PUBLIC srv_ipc_module)

add_library(audio_srv_module STATIC
    srv/audio/AudioSrv.cpp
    srv/audio/AudioSrvApp.cpp
)
target_include_directories(audio_srv_module PUBLIC
    srv/audio
    core
    core/callback
    driver
)
target_link_libraries(audio_srv_module PUBLIC srv_ipc_module)

# Executables (main.cpp contains only main())
add_executable(app app/main.cpp)
target_include_directories(app PRIVATE
    app
    app/adapters
    core
    core/callback
    core/manager
    srv/led
    srv/audio
    driver
)
target_link_libraries(app PRIVATE app_module)

add_executable(srv_manager core/manager/main.cpp)
target_include_directories(srv_manager PRIVATE
    core
    core/manager
    driver
)
target_link_libraries(srv_manager PRIVATE srv_manager_module)

add_executable(led_srv srv/led/main.cpp)
target_include_directories(led_srv PRIVATE
    srv/led
    core
    core/callback
    driver
)
target_link_libraries(led_srv PRIVATE led_srv_module)

add_executable(audio_srv srv/audio/main.cpp)
target_include_directories(audio_srv PRIVATE
    srv/audio
    core
    core/callback
    driver
)
target_link_libraries(audio_srv PRIVATE audio_srv_module)
