# Experiment1/CMakeLists.txt
add_executable(LexicalAnalyzer LexicalAnalyzer.cpp)

# 指定编译后的可执行文件输出目录
set_target_properties(LexicalAnalyzer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build
)
