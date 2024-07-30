cmake_minimum_required(VERSION 3.28)
project(RayTracing)

set(CMAKE_CXX_STANDARD 11)

add_executable(RayTracing src/test1.cpp src/glad.c )
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include include/assimp/config.h)
link_directories(${PROJECT_SOURCE_DIR}/lib)
file(GLOB LIBRARIES "${PROJECT_SOURCE_DIR}/lib/*.dll")
target_link_libraries(RayTracing ${LIBRARIES})

file(GLOB res "${PROJECT_SOURCE_DIR}/Resources/*")
file(REMOVE_RECURSE "${PROJECT_BINARY_DIR}/Resources/*")
file(COPY ${res} DESTINATION ${PROJECT_BINARY_DIR}/Resources)

