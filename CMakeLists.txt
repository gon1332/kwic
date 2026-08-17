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
        details.h
        details.cpp
)
target_link_libraries(kwic fmt::fmt)


