#include <iostream>
#include <opencv2/opencv.hpp>
#include <cstdlib>

int main(int argc, char*argv[]) {
    // 打开摄像头
    char* camera = argv[1];
    cv::VideoCapture cap;
    
    cap.open((int)(camera[0] - '0'), cv::CAP_V4L2);
    if (!cap.isOpened()) {
        std::cerr << "Failed to open camera." << std::endl;
        return -1;
    }

    // 设置摄像头参数
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    // cap.set(cv::CAP_PROP_FPS, 30);
    // cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('Y', 'U', 'Y', 'V'));

    // FFmpeg命令行参数
    const char* ffmpegCommand = "/usr/local/ffmpeg/bin/ffmpeg -re -f rawvideo -pix_fmt bgr24 -s 640x480 -i - -c:v libx264 -pix_fmt bgr24 -preset veryfast -tune zerolatency -f rtsp -rtsp_transport tcp rtsp://10.42.0.1:8554/camera";
    // const char* ffmpegCommand = "/usr/local/ffmpeg/bin/ffmpeg -f rawvideo -pix_fmt yuv444p -s 640x480 -i - -c:v libx264 -pix_fmt yuv444p -f rtsp -rtsp_transport tcp rtsp://10.42.0.1:8554/camera";
    // 输出FFmpeg命令
    std::cout << "Running command: " << ffmpegCommand << std::endl;

    // 使用popen创建FFmpeg进程
    FILE* ffmpegPipe = popen(ffmpegCommand, "w");
    if (!ffmpegPipe) {
        std::cerr << "Failed to open pipe for pushing stream." << std::endl;
        return -1;
    }
    while (true) {
        cv::Mat frame;
        cap.read(frame);
        if (frame.empty()) {
            std::cerr << "Failed to grab frame from camera." << std::endl;
            break;
        }

        // 将帧数据写入FFmpeg进程的标准输入
        fwrite(frame.data, 1, frame.total() * frame.elemSize(), ffmpegPipe);

        if (cv::waitKey(1) == 'q') break;
    }

    // 关闭FFmpeg进程
    pclose(ffmpegPipe);
    cap.release();
    return 0;
}