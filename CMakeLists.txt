cmake_minimum_required(VERSION 3.10)
project(mini_jvm VERSION 0.1.0)

set(CMAKE_C_STANDARD 99)

find_package(PkgConfig REQUIRED)
find_package(ICU COMPONENTS uc REQUIRED)
pkg_check_modules(GLIB REQUIRED IMPORTED_TARGET glib-2.0)

# The logging library from https://github.com/rxi/log.c.
add_library(logging_lib
    src/log.c
)

target_include_directories(logging_lib
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

add_compile_definitions(LOG_USE_COLOR)

# The code for `mini-jvm` is built into the `mini-jvm-lib` library before
# linking an executable target for it.
add_library(mini_jvm_lib
    src/class_loader/class_flags.c
    src/class_loader/class_format.c
    src/class_loader/class_obj.c
    src/class_loader/class_reader.c
    src/constant_pool/constant_pool.c
    src/constant_pool/link_resolver.c
    src/kernel/class_pool.c
    src/utils/file_io.c
    src/utils/sreader.c
    src/utils/string_format.c
)

target_include_directories(mini_jvm_lib
    PUBLIC
        ${GLIB_INCLUDE_DIRS}
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(mini_jvm_lib PkgConfig::GLIB logging_lib ICU::uc)

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

set_target_properties(mini_jvm_lib PROPERTIES C_EXTENSIONS OFF)
