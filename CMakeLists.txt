cmake_minimum_required(VERSION 3.28)
project(RayTracing)

set(CMAKE_CXX_STANDARD 11)

add_executable(RayTracing main.cpp src/glad.c)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)
target_link_libraries(RayTracing ${PROJECT_SOURCE_DIR}/lib/glfw3.dll)
