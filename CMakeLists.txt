cmake_minimum_required (VERSION 3.5)
project (blackscholes CXX)

# Set global project finders
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Set (CMAKE_C_STANDARD 99)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set (CMAKE_BUILD_TYPE Debug)

# Find packages
find_package (Threads REQUIRED)
find_package (Boost REQUIRED)
find_package (Glog REQUIRED)

# Set libraries
set (TURKEY_LIBRARIES
	m
	glog
	${CMAKE_THREAD_LIBS_INIT}
	)

# Current source dir alias
set (SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Set include directories
include_directories (
	${SRC}
	${Boost_INCLUDE_DIR}
	)

# Client application
add_executable (${PROJECT_NAME}_client ${SRC}/main_client.cpp)
target_link_libraries(${PROJECT_NAME}_client ${TURKEY_LIBRARIES})

# Server application
set (SERVER_SOURCE_FILES
	${SRC}/main_server.cpp
	${SRC}/Server.cpp
	${SRC}/ProcReader.cpp
	)

add_executable (${PROJECT_NAME}_server ${SERVER_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME}_server ${TURKEY_LIBRARIES})
