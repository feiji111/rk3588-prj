cmake_minimum_required(VERSION 3.0.0)
project(rknn VERSION 0.1.0)

find_package(OpenCV REQUIRED)

message(STATUS "    version:${OpenCV_VERSION}")
message(STATUS "    libraries:${OpenCV_LIBS}")
message(STATUS "    include_path:${OpenCV_INCLUDE_DIRS}")

include_directories(${PROJECT_SOURCE_DIR}/include)

set(dynamic_libs
    ${PROJECT_SOURCE_DIR}/lib/librknnrt.so
    ${OpenCV_LIBS}
)

aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC_DIR)

add_executable(rknn ${PROJECT_SOURCE_DIR}/main.cpp ${SRC_DIR})
add_definitions("-Wall -g")

target_link_libraries(rknn ${dynamic_libs})