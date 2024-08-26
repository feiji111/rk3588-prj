#include <iostream>
#include <opencv2/opencv.hpp>
#include <cstdlib>
#include <vector>

int main(int argc, char**argv) {
    // FFmpeg命令行参数
    if(argc < 4) {
        printf("Usage %s <RTSP-url> <width> <height>\n", argv[0]);
    }

    std::string RTSP_url = argv[1];
    std::string ffmpegCommand = "/usr/local/ffmpeg/bin/ffmpeg -c:v h264 -i " + RTSP_url +  " -f rawvideo -pix_fmt bgr24 -";
    // const char* ffmpegCommand = "/usr/local/ffmpeg/bin/ffmpeg -i rtsp://10.42.0.1:8554/camera -f rawvideo -pix_fmt bgr24 -";

    // 输出FFmpeg命令
    std::cout << "Running command: " << ffmpegCommand << std::endl;

    // 使用popen创建FFmpeg进程
    FILE* ffmpegPipe = popen(ffmpegCommand.c_str(), "r");
    if (!ffmpegPipe) {
        std::cerr << "Failed to open pipe for pulling stream." << std::endl;
        return -1;
    }

    // 读取FFmpeg输出并使用OpenCV显示
    const int width = atoi(argv[2]);
    const int height = atoi(argv[3]);
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