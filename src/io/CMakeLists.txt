add_library(mss_io UserIOController.cpp)

target_include_directories(mss_io PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(mss_io PUBLIC mss_core)
target_compile_options(mss_io PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic;-Werror>
)
