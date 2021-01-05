
cmake_minimum_required(VERSION 3.4.1)


set(vm_files
        src/main/cpp/jin_load.cpp
        src/main/cpp/chip/vmchip.cpp
        src/main/cpp/chip/chip8.cpp
        )
add_library(
        vmchip8-lib
        SHARED
        ${vm_files}
)


find_library(
        log-lib
        log)


target_link_libraries(
        vmchip8-lib
        ${log-lib})

