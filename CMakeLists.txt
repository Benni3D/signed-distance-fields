cmake_minimum_required(VERSION 3.17)
project(sdf)

set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_CXX_FLAGS -mtune=native)
set(CMAKE_CXX_STANDARD 20)
find_package(SDL2 REQUIRED)
find_package(spdlog REQUIRED)
find_package(Threads REQUIRED)
include_directories(include)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(sdf src/main.cpp include/math.hpp include/shader.hpp include/raytracer.hpp include/object.hpp src/objects.cpp src/rt.cpp src/shaders.cpp include/screen.hpp src/screen.cpp)
target_link_libraries(sdf SDL2 spdlog Threads::Threads atomic)