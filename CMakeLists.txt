cmake_minimum_required(VERSION 3.20)
project(Plamol LANGUAGES C)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.c")

option(FETCH_RAYLIB "Download and build raylib if it isn't installed" OFF)
set(RAYLIB_DIR "" CACHE PATH "Path to a local raylib install (containing lib/cmake/raylib)")

if (RAYLIB_DIR)
    list(APPEND CMAKE_PREFIX_PATH "${RAYLIB_DIR}")
endif()

find_package(raylib 5.0 QUIET)

if (NOT raylib_FOUND)
    if (FETCH_RAYLIB)
        include(FetchContent)
        FetchContent_Declare(
            raylib
            GIT_REPOSITORY https://github.com/raysan5/raylib.git
            GIT_TAG 6.0
        )
        FetchContent_MakeAvailable(raylib)
    else()
        message(FATAL_ERROR
            "raylib not found.\n"
            "  - Install it, or\n"
            "  - Point to it:   cmake -S . -B build -DRAYLIB_DIR=/path/to/raylib/install\n"
            "  - Or download:   cmake -S . -B build -DFETCH_RAYLIB=ON \n")
    endif()
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
foreach (cfg DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${cfg} ${CMAKE_SOURCE_DIR})
endforeach()

add_executable(app ${SOURCES})
target_include_directories(app PRIVATE src)
target_link_libraries(app PRIVATE raylib)
