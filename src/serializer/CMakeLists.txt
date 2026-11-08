add_library(mss_serializer
    ConfigurationSerializer.cpp
    FileExplorer.cpp
    SerializerTool.cpp
)
target_include_directories(mss_serializer PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(mss_serializer PUBLIC mss_core)
target_compile_options(mss_serializer PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic;-Werror>
)
