cmake_minimum_required(VERSION 3.19)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(GCC_COVERAGE_COMPILE_FLAGS "-lglfw -lGLU -lGL -lGLEW -Wall -O0 -lglew")

project(SpaceInvaders)

find_package(GLEW REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})
add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${GLEW_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE GLEW::GLEW ${GLFW_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)