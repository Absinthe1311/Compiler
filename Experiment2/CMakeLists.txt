# 创建可执行文件
add_executable(SyntaxAnalyzer SyntaxAnalyzer.cpp)

# 设置目标属性
target_compile_features(SyntaxAnalyzer PRIVATE cxx_std_17)

# 编译选项
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(SyntaxAnalyzer PRIVATE -g -O0 -Wall -Wextra)
else()
    target_compile_options(SyntaxAnalyzer PRIVATE -O2)
endif()

