cmake_minimum_required(VERSION 3.10)
project(UDPPush)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

link_directories("/usr/local/x264/lib")

# 查找 FFmpeg 库
find_package(PkgConfig REQUIRED)
find_package(OpenCV REQUIRED)
pkg_check_modules(FFMPEG REQUIRED libavformat libavcodec libavutil libswscale)
pkg_check_modules(X264 REQUIRED x264)

# 添加包含目录
include_directories(${FFMPEG_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS} ${X264_INCLUDE_DIRS})
# include_directories(${OpenCV_INCLUDE_DIRS} ${X264_INCLUDE_DIRS})
# debug
add_definitions("-g")

# 添加可执行文件
add_executable(push push-v2.cpp)

# 链接 FFmpeg 库
target_link_libraries(push ${X264_LIBRARIES} ${OpenCV_LIBRARIES} ${FFMPEG_LIBRARIES} )
# target_link_libraries(push ${X264_LIBRARIES} ${OpenCV_LIBRARIES})