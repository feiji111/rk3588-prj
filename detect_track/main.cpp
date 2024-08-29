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
#include "model.h"
#include "rknnPool.hpp"
#include "rknn_api.h"
#include "mpp/mpp-v2.h"
#include "command.h"

extern AVPacket *packet;

struct MouseData {
    frameDets* dets;
    cv::VideoCapture* capture;
    cv::Ptr<cv::TrackerNano>* tracker;
};

bool pointInRectangle(int x, int y, const cv::Rect& rect) 
{
    return (x >= rect.x && x <= (rect.x + rect.width) && y >= rect.y && y <= (rect.y + rect.height));
}

void onMouseClick(int event, int x, int y, int flags, void* userdata) 
{
    MouseData* data = (MouseData*)userdata;
    if (event == cv::EVENT_LBUTTONDOWN) 
    {
        cv::destroyAllWindows();
        cv::VideoCapture capture = *data->capture;
        cv::Ptr<cv::TrackerNano> tracker = *data->tracker;
        frameDets dets =*data->dets;
        cv::Rect bbox;

        for (const auto& rect: dets.dets.results)
        {
            if (pointInRectangle(x, y, rect.box))
            {
                rectangle(dets.img, cv::Point(rect.box.x, rect.box.y), cv::Point(rect.box.x + rect.box.width, rect.box.y + rect.box.height), cv::Scalar(255, 0, 0), 2);
                tracker->init(dets.img, rect.box);

                break;
            }
        }

        while(capture.isOpened()) {
            cv::Mat img;
            if (capture.read(img) == false)
                break;
            
            tracker->update(dets.img, bbox);            
            cv::rectangle(dets.img, bbox, cv::Scalar(0, 0, 0), 3);
            cv::imshow("Camera FPS", dets.img);
            if (cv::waitKey(1) == 'q')
                break;
        }
        cv::destroyAllWindows();
    }
}

int main(int argc, char **argv)
{
    Command cmd = process_command(argc, argv);
    // if (argc != 6)
    // {
    //     printf("Usage: %s <detection_model> <cameraID/video_path> <RTSP-url> <width> <height>\n", argv[0]);
    //     return -1;
    // }
    //push stream
    // std::string rtsp_dest = argv[3];
    // std::string width = argv[4];
    // std::string height = argv[5];
    // std::string resolution = width + 'x' + height;
    int int_width, int_height;
    int_width = cmd.getWidth();
    int_height = cmd.getHeight();

    // std::string ffmpegCommand = "/usr/local/ffmpeg/bin/ffmpeg -re -f rawvideo -pix_fmt bgr24 -s " + resolution + " -i - -c:v libx264 -r 25 -pix_fmt yuv420p -preset veryfast -tune zerolatency -f rtsp -rtsp_transport udp " + rtsp_dest;
    // std::string ffmpegCommand = "/usr/local/ffmpeg/bin/ffmpeg -f rawvideo -pix_fmt yuv420p -r 25 -s " + resolution + " -i - -c:v copy -f rtsp -rtsp_transport udp " + rtsp_dest;
    // std::string ffmpegCommand = "/usr/local/ffmpeg/bin/ffmpeg -r 60 -i - -r 60 -c:v copy -f rtsp -rtsp_transport tcp " + rtsp_dest;

    // printf("Running command: %s\n", ffmpegCommand.c_str());

    // FILE* ffmpegPipe = popen(ffmpegCommand.c_str(), "w");
    // if(!ffmpegPipe) {
    //     fprintf(stderr, "Failed to open pipe for pushing stream.\n");
    //     return -1;
    // }

    //创建Linux FIFO文件
    // ::unlink("./test.264");
    // ::mkfifo("./test.264", O_CREAT | O_EXCL | 777);
    // FILE *fp_output = fopen("./test.264", "w+b");

    std::string detection_model = cmd.getDetectionModel();
    std::string track_model_head = cmd.getTrackModelHead();
    std::string track_model_backbone = cmd.getTrackModelBackbone();

    int threadNum = 6;
    rknnPool<rkYolov5s, cv::Mat, frameDets> testPool(detection_model, threadNum);
    // rknnPool<FeatureTensor, cv::Mat, cv::Mat> reidPool(trk_model, threadNum);
    
    if (testPool.init() != 0)
    {
        printf("rknnPool init fail!\n");
        return -1;
    }

    //init NanoTracker
    cv::TrackerNano::Params params;
    params.backbone = track_model_head.c_str();
    params.neckhead = track_model_backbone.c_str();

    cv::Ptr<cv::TrackerNano> tracker = cv::TrackerNano::create(params);

    //init mouse
    MouseData mouseData;
    mouseData.tracker = &tracker;

    cv::namedWindow("Camera");
    cv::setMouseCallback("Camera", onMouseClick, &mouseData);

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

    while (capture.isOpened())
    {  
        cv::Mat img;
        frameDets dets;
        if (capture.read(img) == false)
            break;
        dets.img = img;
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
        mouseData.dets = &dets;
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


        cv::Mat resultYUV;

        cv::cvtColor(dets.img, resultYUV, cv::COLOR_BGR2YUV_YV12);
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

        // if(mouseData.clicked)
        // {
        //     // cout << mouseData.x << " " << mouseData.y << endl;
        //     for (const auto& rect: dets.dets.results)
        //     {
        //         if (pointInRectangle(mouseData.x, mouseData.y, rect.box))
        //         {
        //             rectangle(img, cv::Point(rect.box.x, rect.box.y), cv::Point(rect.box.x + rect.box.width, rect.box.y + rect.box.height), cv::Scalar(255, 0, 0), 2);
        //             tracker->init(dets.img, rect.box);

        //             isTracking = true;
        //             break;
        //         }
        //     }

        // }

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
