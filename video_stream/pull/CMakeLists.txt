cmake_minimum_required(VERSION 3.10)
project(UDPStream)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 查找 FFmpeg 库
find_package(PkgConfig REQUIRED)
find_package(OpenCV REQUIRED)
pkg_check_modules(FFMPEG REQUIRED libavformat libavcodec libavutil libswscale)
# pkg_check_modules(OpenCV REQUIRED)

# 添加包含目录
include_directories(${FFMPEG_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})

add_definitions("-g")


# 添加可执行文件
add_executable(pull pull-v2.cpp)

# 链接 FFmpeg 库
target_link_libraries(pull ${FFMPEG_LIBRARIES} ${OpenCV_LIBRARIES})