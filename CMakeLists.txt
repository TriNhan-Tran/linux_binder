cmake_minimum_required(VERSION 3.16)
project(binder_ipc C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic -O2 -g)

# Binder kernel driver C library
add_library(binder_driver STATIC driver/binder.c)
target_include_directories(binder_driver PUBLIC driver)

# Core module (binder wrappers + parcel)
add_library(core_module STATIC
    core/BinderCore.cpp
    core/Parcel.cpp
)
target_include_directories(core_module PUBLIC core driver)
target_link_libraries(core_module PUBLIC binder_driver pthread)

# Binder srv IPC module
add_library(srv_ipc_module STATIC
    srv/SrvManagerIpc.cpp
    srv/LedSrvIpc.cpp
    srv/AudioSrvIpc.cpp
    srv/SrvCallbackIpc.cpp
    srv/SrvCallbackRegistry.cpp
)
target_include_directories(srv_ipc_module PUBLIC core srv driver)
target_link_libraries(srv_ipc_module PUBLIC core_module)

# App module implementation
add_library(app_module STATIC
    app/AppCallback.cpp
    app/AppRunner.cpp
)
target_include_directories(app_module PUBLIC app core srv driver)
target_link_libraries(app_module PUBLIC srv_ipc_module)

# Srv process modules
add_library(srv_manager_module STATIC
    srv/manager/SrvManagerHost.cpp
    srv/manager/SrvManagerApp.cpp
)
target_include_directories(srv_manager_module PUBLIC srv/manager core srv driver)
target_link_libraries(srv_manager_module PUBLIC srv_ipc_module)

add_library(led_srv_module STATIC
    srv/led/LedSrvHost.cpp
    srv/led/LedSrvApp.cpp
)
target_include_directories(led_srv_module PUBLIC srv/led core srv driver)
target_link_libraries(led_srv_module PUBLIC srv_ipc_module)

add_library(audio_srv_module STATIC
    srv/audio/AudioSrvHost.cpp
    srv/audio/AudioSrvApp.cpp
)
target_include_directories(audio_srv_module PUBLIC srv/audio core srv driver)
target_link_libraries(audio_srv_module PUBLIC srv_ipc_module)

# Executables (main.cpp contains only main())
add_executable(app app/main.cpp)
target_include_directories(app PRIVATE app core srv driver)
target_link_libraries(app PRIVATE app_module)

add_executable(srv_manager srv/manager/main.cpp)
target_include_directories(srv_manager PRIVATE srv/manager core srv driver)
target_link_libraries(srv_manager PRIVATE srv_manager_module)

add_executable(led_srv srv/led/main.cpp)
target_include_directories(led_srv PRIVATE srv/led core srv driver)
target_link_libraries(led_srv PRIVATE led_srv_module)

add_executable(audio_srv srv/audio/main.cpp)
target_include_directories(audio_srv PRIVATE srv/audio core srv driver)
target_link_libraries(audio_srv PRIVATE audio_srv_module)
