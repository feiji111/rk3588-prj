#include <iostream>
#include <opencv2/opencv.hpp>
#include <cstdlib>
#include <vector>

int main() {
    // FFmpeg命令行参数
    const char* ffmpegCommand = "/usr/local/ffmpeg/bin/ffmpeg -i udp://localhost:1234 -f rawvideo -pix_fmt bgr24 -";

    // 输出FFmpeg命令
    std::cout << "Running command: " << ffmpegCommand << std::endl;

    // 使用popen创建FFmpeg进程
    FILE* ffmpegPipe = popen(ffmpegCommand, "r");
    if (!ffmpegPipe) {
        std::cerr << "Failed to open pipe for pulling stream." << std::endl;
        return -1;
    }

    // 读取FFmpeg输出并使用OpenCV显示
    const int width = 640;
    const int height = 480;
    const int channels = 3; // YUYV422 has 2 channels
    std::vector<uchar> buffer(width * height * channels);
    cv::Mat bgrFrame;

    while (true) {
        size_t bytesRead = fread(buffer.data(), 1, buffer.size(), ffmpegPipe);
        if (bytesRead != buffer.size()) {
            std::cerr << "Failed to read frame data from pipe." << std::endl;
            break;
        }

        // 将读取的数据转换为OpenCV Mat
        bgrFrame = cv::Mat(height, width, CV_8UC3, buffer.data()).clone();

        // 显示帧
        cv::imshow("Pulled Stream", bgrFrame);
        if (cv::waitKey(30) >= 0) break;
    }

    // 关闭FFmpeg进程
    pclose(ffmpegPipe);
    cv::destroyAllWindows();
    return 0;
}