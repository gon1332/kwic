cmake_minimum_required(VERSION 3.25)
project(kwic)

# --- Settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# --- Options
option(KWIC_ENABLE_FUZZING "Build libFuzzer harnesses (requires Clang)" OFF)

# --- Includes
include(FetchContent)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt
        GIT_TAG        e69e5f977d458f2650bb346dadf2ad30c5320281 # 10.2.1
        SYSTEM)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
        cli11
        GIT_REPOSITORY https://github.com/CLIUtils/CLI11
        GIT_TAG        v2.4.2
        SYSTEM)
FetchContent_MakeAvailable(cli11)

# -- Targets
add_executable(kwic
        main.cpp
        kwic.h
        shared_data.cpp
        abstract_data.cpp
        implicit_invocation.cpp
        filters.cpp
        details.h
        details.cpp
)
target_link_libraries(kwic fmt::fmt CLI11::CLI11)
target_compile_options(kwic PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wconversion
        -Wsign-conversion
        -Wcast-align
        -Wold-style-cast
        -Wnon-virtual-dtor
        -Woverloaded-virtual
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2
        -Wimplicit-fallthrough
        -Wunused
)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(kwic PRIVATE
            -Wuseless-cast
            -Wlogical-op
            -Wduplicated-cond
            -Wduplicated-branches
            -Wmisleading-indentation
    )
endif()

if(KWIC_ENABLE_FUZZING)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(FATAL_ERROR "Fuzzing requires Clang (got ${CMAKE_CXX_COMPILER_ID}); configure with -DCMAKE_CXX_COMPILER=clang++")
    endif()

    add_executable(kwic_fuzz_tokenize_line
            fuzz/fuzz_tokenize_line.cpp
            details.cpp
            details.h
    )
    target_include_directories(kwic_fuzz_tokenize_line PRIVATE ${CMAKE_SOURCE_DIR})
    target_compile_options(kwic_fuzz_tokenize_line PRIVATE -fsanitize=fuzzer,address,undefined -fno-sanitize-recover=all -g -O1)
    target_link_options(kwic_fuzz_tokenize_line PRIVATE -fsanitize=fuzzer,address,undefined -fno-sanitize-recover=all)
endif()
