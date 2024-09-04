#include <bits/types/struct_timeval.h>
#include <cstdlib>
#include <opencv2/videoio.hpp>
#include <stdio.h>
#include <sys/time.h>

#include <memory>

//OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video/tracking.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
//FFmpeg
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavcodec/packet.h>
}

#include "box.h"
#include "common.h"
#include "model-detection.h"
#include "inference-classification.h"
#include "preprocess-classification.h"
#include "postprocess-classification.h"
#include "rknnPool.hpp"
#include "rknn_api.h"
#include "mpp/mpp-v2.h"
#include "command.h"

extern AVPacket *packet;

bool isTracking = false;

struct MouseData {
    bool clicked;
    int x, y;
};

bool pointInRectangle(int x, int y, const cv::Rect& rect) 
{
    return (x >= rect.x && x <= (rect.x + rect.width) && y >= rect.y && y <= (rect.y + rect.height));
}

char *label[] = {"airplane", "automobile", "bird", "cat", "deer", "dog", "frog", "horse", "ship", "truck"};

int main(int argc, char **argv)
{
    Command cmd = process_command(argc, argv);
    std::string mode = cmd.getMode();

    int int_width, int_height;
    int_width = cmd.getWidth();
    int_height = cmd.getHeight();


    //创建Linux FIFO文件
    // ::unlink("./test.264");
    // ::mkfifo("./test.264", O_CREAT | O_EXCL | 777);
    // FILE *fp_output = fopen("./test.264", "w+b");

    std::string binary_model = cmd.getBinaryModel();
    std::string detection_model = cmd.getDetectionModel();
    std::string track_model_head = cmd.getTrackModelHead();
    std::string track_model_backbone = cmd.getTrackModelBackbone();

    int threadNum = 6;
    rknnPool<rkYolov5s, cv::Mat, frameDets> testPool(detection_model, threadNum);
    
    if (testPool.init() != 0)
    {
        printf("rknnPool init fail!\n");
        return -1;
    }

    //init classification model
    rknn_app_context_t app_ctx;
    init_model(binary_model.c_str(), &app_ctx);

    //init NanoTracker
    cv::TrackerNano::Params params;
    params.backbone = track_model_head.c_str();
    params.neckhead = track_model_backbone.c_str();

    cv::Ptr<cv::TrackerNano> tracker = cv::TrackerNano::create(params);

    //init mouse
    MouseData mouseData;

    cv::namedWindow("Camera");

    cv::VideoCapture capture;
    capture = capture_init(cmd);

    #ifdef DEBUG
    struct timeval time, time1, io_time;
    gettimeofday(&time, nullptr);
    auto startTime = time.tv_sec * 1000 + time.tv_usec / 1000;
    
    auto beforeTime = startTime;
    #endif

    int frames = 0;
    cv::Rect bbox;

    rknn_output **outputs;
    outputs = (rknn_output**)malloc(sizeof(rknn_output*));

    while (capture.isOpened())
    {  
        cv::Mat img, output, img_rgb;
        frameDets dets;
        if (capture.read(img) == false)
            break;
        dets.img = img;
        
        if(mode == "detection") {
            #ifdef DEBUG
            gettimeofday(&time1, nullptr);
            auto beforeInference = time1.tv_sec * 1000 + time1.tv_usec / 1000;
            #endif

            if (testPool.put(dets.img) != 0)
                    break;
            
            if (frames >= threadNum && testPool.get(dets) != 0)
                break;
        
            //draw box
            char text[256];
            char fps[256];

            for (int i = 0; i < dets.dets.results.size(); i++) {
                DetectBox *det_result = &(dets.dets.results[i]);
                sprintf(text, "%.1f%%", det_result->confidence * 100);
                int x1 = det_result->box.x;
                int y1 = det_result->box.y;
                int x2 = x1 + det_result->box.width;
                int y2 = y1 + det_result->box.height;
                rectangle(dets.img, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 0, 0), 3);
                putText(dets.img, text, cv::Point(x1, y1 + 12), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(0, 255, 0));
            }
            // cv::rectangle(dets.img, bbox, cv::Scalar(0, 0, 0), 3);         
            #ifdef DEBUG
            gettimeofday(&time1, nullptr);
            auto afterInference = time1.tv_sec * 1000 + time1.tv_usec / 1000;
            
            sprintf(fps, "FPS: %d", (int)(1000.0 / float(afterInference - beforeInference)));
            
            cv::putText(dets.img, fps, cv::Point(16, 32), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 2);
            // cv::imshow("Camera", dets.img);
            
            #endif
        } else if(mode == "classification") {
            cv::cvtColor(dets.img, img_rgb, cv::COLOR_BGR2RGB);
            // fwrite(img.data, 1, img.total() * img.elemSize(), ffmpeg_process);

            output = pre_process(img_rgb, app_ctx);
            
            inference_model(&app_ctx, output, outputs);

            int result = post_process(&app_ctx, *outputs);
            if(result < 0) {
                printf("error result!\n");
            } else {
                printf("prediction: %s\n", label[result]);
            }            
        }


        cv::Mat resultYUV;

        cv::cvtColor(dets.img, resultYUV, cv::COLOR_RGB2YUV_YV12);
        // uchar *h264_buf = nullptr;
        // int buf_len = 0;
        // YuvtoH264(int_width, int_height, resultYUV, h264_buf, buf_len); 
        // std::cout << dets.img.rows << ' ' << dets.img.cols << std::endl;
        // gettimeofday(&io_time, nullptr);
        // auto before_io = io_time.tv_sec * 1000 + io_time.tv_usec / 1000;
        // fwrite(dets.img.data, 1, dets.img.total() * dets.img.elemSize(), ffmpegPipe);

        // fwrite(h264_buf, 1, buf_len, ffmpegPipe);
        transfer_frame(resultYUV, cmd);

        // Write the compressed frame to the media file
        // if (av_interleaved_write_frame(output_format_context, &packet) < 0) {
        //     fprintf(stderr, "Error while writing output packet\n");
        //     return -1;
        // }

        // av_packet_unref(&packet);

        // gettimeofday(&io_time, nullptr);
        // auto after_io = io_time.tv_sec * 1000 + io_time.tv_usec / 1000;

        // printf("i/o time: %ldms\n", after_io - before_io);
        // delete h264_buf;
        // h264_buf = nullptr;

        av_packet_unref(packet);

        if (cv::waitKey(1) == 'q') // delay 1ms
            break;
        frames++;

        if(mouseData.clicked)
        {
            // cout << mouseData.x << " " << mouseData.y << endl;
            for (const auto& rect: dets.dets.results)
            {
                if (pointInRectangle(mouseData.x, mouseData.y, rect.box))
                {
                    rectangle(img, cv::Point(rect.box.x, rect.box.y), cv::Point(rect.box.x + rect.box.width, rect.box.y + rect.box.height), cv::Scalar(255, 0, 0), 2);
                    tracker->init(dets.img, rect.box);

                    isTracking = true;
                    break;
                }
            }

        }

        #ifdef DEBUG
        if (frames % 120 == 0)
        {
            gettimeofday(&time, nullptr);
            auto currentTime = time.tv_sec * 1000 + time.tv_usec / 1000;
            printf("120帧内平均帧率:\t %f fps/s\n", 120.0 / float(currentTime - beforeTime) * 1000.0);
            beforeTime = currentTime;
        }
        #endif
    }

    // 清空rknn线程池/Clear the thread pool
    while (true)
    {
        frameDets dets;
        if (testPool.get(dets) != 0)
            break;
        cv::imshow("Camera FPS", dets.img);
        if (cv::waitKey(1) == 'q') // 延时1毫秒,按q键退出/Press q to exit
            break;
        frames++;
    }
    #ifdef DEBUG
    gettimeofday(&time, nullptr);
    auto endTime = time.tv_sec * 1000 + time.tv_usec / 1000;

    printf("Average:\t %f fps/s\n", float(frames) / float(endTime - startTime) * 1000.0);
    #endif

    // pclose(ffmpegPipe);
    capture.release();
    destory();

    return 0;
}
