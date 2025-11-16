cmake_minimum_required(VERSION 3.10)
project(SyntaxAnalyzer)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加可执行文件
add_executable(SyntaxAnalyzer SyntaxAnalyzer.cpp)