#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <string.h>
#include <stdio.h>
#include <iostream>

#include <sys/time.h>

#include <vector>
#include <string>

//opencv
// #include "opencv2/core.hpp"
// #include "opencv2/videoio.hpp"
// #include "opencv2/imgcodecs.hpp"
#include "opencv2/opencv.hpp"
//ffmpeg
#include "libavcodec/avcodec.h"
#include "rknn_api.h"

#include "inference.h"
#include "postprocess.h"
#include "preprocess.h"

char *label[] = {"airplane", "automobile", "bird", "cat", "deer", "dog", "frog", "horse", "ship", "truck"};

int main(int argc, char*argv[]) {
    char *model_path, *input_source;

    if(argc < 3) {
        printf("Usage: %s <rknn_model_path> <intput_source>\n", argv[0]);
        return -1;
    }

    model_path = argv[1];
    input_source = argv[2];

    rknn_app_context_t app_ctx;
    init_model(model_path, &app_ctx);

    cv::namedWindow("Camera");
    cv::namedWindow("After resize");

    
    cv::VideoCapture capture;
    
    if(strlen(input_source) == 1) {
        capture.open((int)(input_source[0] - '0'), cv::CAP_V4L2);
        capture.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
        capture.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
        capture.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);
    } else {
        capture.open(input_source, cv::CAP_FFMPEG);
    }
    

    int frame_width = static_cast<int>(capture.get(cv::CAP_PROP_FRAME_WIDTH));
    int frame_height = static_cast<int>(capture.get(cv::CAP_PROP_FRAME_HEIGHT));

    cv::Mat img, output, img_rgb;
    rknn_output **outputs;
    outputs = (rknn_output**)malloc(sizeof(rknn_output*));

    struct timeval time;
    gettimeofday(&time, nullptr);
    auto startTime = time.tv_sec * 1000 + time.tv_usec / 1000;
    auto beforeTime = startTime;

    int frame = 0;

    while(capture.isOpened()) {
        if(!capture.read(img)) {
            break;
        }

        cv::cvtColor(img, img_rgb, cv::COLOR_BGR2RGB);
        // fwrite(img.data, 1, img.total() * img.elemSize(), ffmpeg_process);

        output = pre_process(img, app_ctx);
        
        inference_model(&app_ctx, output, outputs);

        int result = post_process(&app_ctx, *outputs);
        if(result < 0) {
            printf("error result!\n");
        } else {
            // printf("prediction: %s\n", label[result]);
        }
        cv::imshow("Camera", img);
        cv::imshow("After resize", output);
        if(cv::waitKey(1) == 'q') {
            break;
        }
        frame++;

        if(frame % 120 == 0) {
            gettimeofday(&time, nullptr);
            auto currentTime = time.tv_sec * 1000 + time.tv_usec / 1000;
            printf("120帧内平均帧率:\t %f fps/s\n", 120.0 / float(currentTime - beforeTime) * 1000.0);
            beforeTime = currentTime;
        }
    }
    rknn_outputs_release(app_ctx.rknn_ctx, app_ctx.io_num.n_output, *outputs);
    free(*outputs);
    return 0;
}