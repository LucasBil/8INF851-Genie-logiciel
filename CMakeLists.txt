cmake_minimum_required(VERSION 3.20)

project(MonSuperSysteme VERSION 0.1.0 LANGUAGES CXX)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

option(ENABLE_COVERAGE "Instrumenter le code pour mesurer la couverture" OFF)
option(MSS_BUILD_TESTS "Construire les tests Google Test" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(ENABLE_COVERAGE AND NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(FATAL_ERROR "ENABLE_COVERAGE nécessite GCC ou Clang.")
endif()

add_subdirectory(src/core)
add_subdirectory(src/io)
add_subdirectory(src/serializer)

add_executable(mss src/main.cpp)
target_link_libraries(mss PRIVATE mss_io mss_core)
target_compile_options(mss PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic;-Werror>
)

if(MSS_BUILD_TESTS)
    enable_testing()
    include(FetchContent)

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    FetchContent_MakeAvailable(googletest)

    add_subdirectory(tests)
endif()

if(ENABLE_COVERAGE)
    foreach(target mss_core mss_io)
        target_compile_options(${target} PRIVATE --coverage -O0 -g)
        target_link_options(${target} PRIVATE --coverage)
    endforeach()
    target_link_options(mss PRIVATE --coverage)
endif()