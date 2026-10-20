add_executable(mss_tests
    core/MatrixTest.cpp
    core/MonSuperSystemeTest.cpp
    core/CpuTest.cpp
    core/RamTest.cpp
    core/OperationTest.cpp
    io/UserIOControllerTest.cpp
)

target_link_libraries(mss_tests PRIVATE mss_core mss_io GTest::gtest_main)
target_compile_options(mss_tests PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic;-Werror>
)

if(ENABLE_COVERAGE)
    target_compile_options(mss_tests PRIVATE --coverage -O0 -g)
    target_link_options(mss_tests PRIVATE --coverage)
endif()

include(GoogleTest)
gtest_discover_tests(mss_tests)
