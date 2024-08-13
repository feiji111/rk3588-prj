#include <iostream>
#include <opencv4/opencv2/core/core.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/videoio.hpp>

int main() {
    cv::VideoCapture capture(0);
    while(capture.isOpened()) {
        cv::Mat frame;
        capture.read(frame);

        imshow("camera", frame);
        if(cv::waitKey(1) == 'q')
            break;
    }
    return 0;
}