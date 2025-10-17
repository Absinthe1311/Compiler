# Experiment1/CMakeLists.txt
add_executable(LexicalAnalyzer LexicalAnalyzer.cpp)

# 可执行文件直接输出到 CMAKE_BINARY_DIR（构建根目录）
set_target_properties(LexicalAnalyzer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)