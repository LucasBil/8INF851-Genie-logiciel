add_library(mss_core
    Matrix.cpp
    MonSuperSysteme.cpp
    Cpu.cpp
    Ram.cpp
)

target_include_directories(mss_core PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_compile_options(mss_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic;-Werror>
)
