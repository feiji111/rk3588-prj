#include <string.h>
#include <stdio.h>

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

    if(strlen(model_path) == 1) {
        //input from camera
        cv::VideoCapture capture;
        capture.open((int)(input_source[0] - '0'), cv::CAP_V4L2);
        capture.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));

        int frame_width = static_cast<int>(capture.get(cv::CAP_PROP_FRAME_WIDTH));
        int frame_height = static_cast<int>(capture.get(cv::CAP_PROP_FRAME_HEIGHT));

        std::string ffmpeg_cmd = "ffmpeg -y -f rawvideo -pix_fmt rgb24 -s " + std::to_string(frame_width) + "x" + std::to_string(frame_height) + 
                             " -r 30 -i - -c:v libx264 -pix_fmt yuv420p -f udp://192.168.5.2:1234";

        FILE *ffmpeg_process = popen(ffmpeg_cmd.c_str(), "w");
        if(!ffmpeg_process) {
            printf("Unable to start a FFmpeg process\n");
        }

        cv::Mat img, output, img_rgb;
        rknn_output *outputs;

        while(capture.isOpened()) {
            if(!capture.read(img)) {
                break;
            }

            cv::cvtColor(img, img_rgb, cv::COLOR_BGR2RGB);
            fwrite(img.data, 1, img.total() * img.elemSize(), ffmpeg_process);

            pre_process(img, app_ctx);
            
            inference_model(&app_ctx, img, outputs);

            int result = post_process(&app_ctx, outputs);
            if(result < 0) {
                printf("error result!\n");
            } else {
                printf("prediction: %s\n", label[result]);
            }
        }
        rknn_outputs_release(app_ctx.rknn_ctx, app_ctx.io_num.n_output, outputs);
        free(outputs);
    } else {
        //input from image
        cv::Mat img = cv::imread(input_source), output;
        if(img.empty()) {
            printf("read image error!\n");
        }
        pre_process(img, app_ctx);

        rknn_output *outputs;
        inference_model(&app_ctx, img, outputs);

        int result = post_process(&app_ctx, outputs);
        if(result < 0) {
            printf("error result!\n");
        } else {
            printf("prediction: %s\n", label[result]);
        }
        rknn_outputs_release(app_ctx.rknn_ctx, app_ctx.io_num.n_output, outputs);
        free(outputs);
    }
}