#include <opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/videoio.hpp>

int main(int argc, char**argv) {
    char* camera = argv[1];
    cv::VideoCapture capture;
    capture.open(camera[0] - '0', cv::CAP_V4L2);
    capture.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    capture.set(cv::CAP_PROP_FRAME_HEIGHT, 720);
    capture.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));

    cv::namedWindow("Camera");
    cv::Mat frame;
    std::cout << capture.get(3) << std::endl;
    std::cout << capture.get(4) << std::endl;
    std::cout << capture.get(5) << std::endl;
    while(capture.isOpened()) {
        capture.read(frame);
        cv::imshow("Camera", frame);
        if (cv::waitKey(1) == 'q') // delay 1ms
            break;
    }
    return 0;
}