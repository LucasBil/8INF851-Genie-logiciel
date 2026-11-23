add_library(mss_logging Logger.cpp)
target_include_directories(mss_logging PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_compile_options(mss_logging PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic;-Werror>
)
