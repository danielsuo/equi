cmake_minimum_required (VERSION 3.5)
project (blackscholes CXX)

# Set global project finders
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} $ENV{TURKEY_HOME}/cmake)

# Set (CMAKE_C_STANDARD 99)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set (CMAKE_BUILD_TYPE Debug)

# Find packages
find_package (Threads)

add_executable (${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
set_target_properties (${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS "ENABLE_THREADS")
target_link_libraries(
  ${PROJECT_NAME}
  m
  ${CMAKE_THREAD_LIBS_INIT}
  )

