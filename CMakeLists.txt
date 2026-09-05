cmake_minimum_required(VERSION 3.25)
project(kwic)

# --- Includes
include(FetchContent)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt
        GIT_TAG        e69e5f977d458f2650bb346dadf2ad30c5320281) # 10.2.1
FetchContent_MakeAvailable(fmt)

# --- Settings
set(CMAKE_CXX_STANDARD 20)

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
target_link_libraries(kwic fmt::fmt)
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
