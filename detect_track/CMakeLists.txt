cmake_minimum_required(VERSION 3.0.0)
project(detect_track VERSION 0.1.0)

# Need C++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(DEBUG "Debug or Release" ON)

if(DEBUG)
    add_definitions("-DDEBUG")
endif(DEBUG)

find_package(PkgConfig REQUIRED)
pkg_check_modules(ROCKCHIP_MPP REQUIRED rockchip_mpp)
message(STATUS "    version: ${ROCKCHIP_MPP_VERSION}")
message(STATUS "    libraries: ${ROCKCHIP_MPP_LIBRARIES}")
message(STATUS "    include path: ${ROCKCHIP_MPP_INCLUDE_DIRS}")

pkg_check_modules(ROCKCHIP_VPU REQUIRED rockchip_vpu)
message(STATUS "    version: ${ROCKCHIP_VPU_VERSION}")
message(STATUS "    libraries: ${ROCKCHIP_VPU_LIBRARIES}")
message(STATUS "    include path: ${ROCKCHIP_VPU_INCLUDE_DIRS}")

find_package(OpenCV REQUIRED)
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBRARIES}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

include_directories(${OpenCV_INCLUDE_DIRS})

set(
    include_libs  
    ${PROJECT_SOURCE_DIR}/include/
    ${PROJECT_SOURCE_DIR}/include/rga/
    ${PROJECT_SOURCE_DIR}/include/rknn_api/
    ${PROJECT_SOURCE_DIR}/include/threadpool/
    ${PROJECT_SOURCE_DIR}/include/mpp/
    ${PROJECT_SOURCE_DIR}/include/osal/
    ${ROCKCHIP_VPU_INCLUDE_DIRS}
    ${ROCKCHIP_MPP_INCLUDE_DIRS}
)

set(
    mpp_include_libs
    /usr/local/include/rockchip/
)

include_directories(${include_libs} ${mpp_include_libs})

aux_source_directory(${PROJECT_SOURCE_DIR}/src DETECTION_SRC_DIR)
add_executable(detect_track ${PROJECT_SOURCE_DIR}/main.cpp ${DETECTION_SRC_DIR})

# 添加动态链接库
set(
    dynamic_libs  
    pthread
    ${PROJECT_SOURCE_DIR}/lib/rknn_api/librknnrt.so
    ${PROJECT_SOURCE_DIR}/lib/rga/librga.so
    ${ROCKCHIP_MPP_LIBRARIES}
    ${ROCKCHIP_VPU_INCLUDE_DIRS}
)

# debug
# add_definitions("-g")

target_link_libraries(detect_track ${OpenCV_LIBS})
target_link_libraries(detect_track ${dynamic_libs})