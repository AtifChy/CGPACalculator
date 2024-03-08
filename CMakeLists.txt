cmake_minimum_required(VERSION 3.27)
project(CGPACalculator)

set(CMAKE_CXX_STANDARD 17)

find_package(nlohmann_json CONFIG REQUIRED)
add_executable(CGPACalculator main.cpp)
target_link_libraries(CGPACalculator PRIVATE nlohmann_json::nlohmann_json)
