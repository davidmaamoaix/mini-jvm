cmake_minimum_required(VERSION 3.10)
project(mini_jvm VERSION 0.1.0)

set(CMAKE_C_STANDARD 99)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED IMPORTED_TARGET glib-2.0)

# The code for `mini-jvm` is built into the `mini-jvm-lib` library before
# linking an executable target for it.
add_library(mini_jvm_lib
    src/utils/sreader.c
)

target_include_directories(mini_jvm_lib
    PUBLIC
        ${GLIB_INCLUDE_DIRS}
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(mini_jvm_lib PkgConfig::GLIB)

# Executable target for `mini-jvm`.
add_executable(mini_jvm src/main.c)

target_link_libraries(mini_jvm
    PRIVATE
        mini_jvm_lib
)

# CPack setup.
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
