#include <bits/types/struct_timeval.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video/tracking.hpp>
#include <stdio.h>
#include <memory>
#include <sys/time.h>

#include "box.h"
#include "common.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include "model-detection.h"
#include "rknnPool.hpp"

// #include "deepsort.h"
#include "rknn_api.h"

bool isTracking = false;
int trk_id = 0;

struct MouseData {
    int x;
    int y;
    bool clicked;
};

void onMouseClick(int event, int x, int y, int flags, void* userdata) 
{
    MouseData* data = (MouseData*)userdata;
    if (event == cv::EVENT_LBUTTONDOWN) 
    {
        data->x = x;
        data->y = y;
        data->clicked = true;
        cv::destroyAllWindows();
    }
}

bool pointInRectangle(int x, int y, const cv::Rect& rect) 
{
    return (x >= rect.x && x <= (rect.x + rect.width) && y >= rect.y && y <= (rect.y + rect.height));
}


int main(int argc, char **argv)
{
    char *model_name = NULL, *trk_model = NULL;
    if (argc != 3)
    {
        printf("Usage: %s <detection_model> <cameraID/video_path>\n", argv[0]);
        return -1;
    }
    // 参数二，检测模型所在路径/The path where the model is located
    model_name = (char *)argv[1];
    // 参数三
    char *vedio_name = argv[2];

    // 初始化rknn线程池/Initialize the rknn thread pool
    int threadNum = 6;
    rknnPool<rkYolov5s, cv::Mat, frameDets> testPool(model_name, threadNum);
    
    if (testPool.init() != 0)
    {
        printf("rknnPool init fail!\n");
        return -1;
    }

    //初始化NanoTracker
    cv::TrackerNano::Params params;
    params.backbone = "../model/nanotrack_backbone_sim.onnx";
    params.neckhead = "../model/nanotrack_head_sim.onnx";

    cv::Ptr<cv::TrackerNano> tracker = cv::TrackerNano::create(params);

    //初始化mouse
    MouseData mouseData;
    mouseData.clicked = false;

    cv::namedWindow("Camera FPS");
    cv::setMouseCallback("Camera FPS", onMouseClick, &mouseData);

    cv::VideoCapture capture;
    if (strlen(vedio_name) == 1) {
        capture.open((int)(vedio_name[0] - '0'), cv::CAP_V4L2);
        capture.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
        capture.set(cv::CAP_PROP_FRAME_WIDTH, 640);
        capture.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    }
    else {
        capture.open(vedio_name, cv::CAP_FFMPEG);
    }

    struct timeval time, time1;
    gettimeofday(&time, nullptr);
    auto startTime = time.tv_sec * 1000 + time.tv_usec / 1000;

    int frames = 0;
    auto beforeTime = startTime;

    cv::Rect bbox;

    while (capture.isOpened())
    {
        cv::Mat img;
        frameDets dets;
        if (capture.read(img) == false)
            break;
        dets.img = img;

        gettimeofday(&time1, nullptr);
        auto beforeInference = time1.tv_sec * 1000 + time1.tv_usec / 1000;
        if(!isTracking) {
            if (testPool.put(dets.img) != 0)
                break;
            
            if (frames >= threadNum && testPool.get(dets) != 0)
                break;
        }
        
        if(isTracking) {
            // double beforeTracking = what_time_is_it_now();
            // track.track_process(dets);
            // double afterTracking = what_time_is_it_now();
            tracker->update(dets.img, bbox);
        }

        // printf("track cost: %fms\n", afterInference - beforeInference);
        //draw box
        char text[256];
        char fps[256];
        if(!isTracking) {            
            for (int i = 0; i < dets.dets.results.size(); i++)
            {
                DetectBox *det_result = &(dets.dets.results[i]);
                // sprintf(text, "%s %.1f%% %d", det_result->name, det_result->confidence * 100, det_result->trackID);
                // sprintf(text, "%.1f%% %d", det_result->confidence * 100, det_result->trackID);
                sprintf(text, "%.1f%%", det_result->confidence * 100);
                int x1 = det_result->box.x;
                int y1 = det_result->box.y;
                int x2 = x1 + det_result->box.width;
                int y2 = y1 + det_result->box.height;
                rectangle(dets.img, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 0, 0), 3);
                putText(dets.img, text, cv::Point(x1, y1 + 12), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(0, 255, 0));
            }
        } else {
            // for (int i = 0; i < dets.dets.results.size(); i++)
            // {
            //     DetectBox *det_result = &(dets.dets.results[i]);
            //     if(det_result->trackID != trk_id) {
            //         continue;
            //     }
            //     // sprintf(text, "%s %.1f%% %d", det_result->name, det_result->confidence * 100, det_result->trackID);
            //     sprintf(text, "%.1f%% %d", det_result->confidence * 100, det_result->trackID);
            //     int x1 = det_result->box.x;
            //     int y1 = det_result->box.y;
            //     int x2 = x1 + det_result->box.width;
            //     int y2 = y1 + det_result->box.height;
            //     rectangle(dets.img, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 0, 0), 3);
            //     putText(dets.img, text, cv::Point(x1, y1 + 12), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(0, 0, 0));
            //     break;
            // }
            cv::rectangle(dets.img, bbox, cv::Scalar(0, 0, 0), 3);
            
        }
        gettimeofday(&time1, nullptr);
        auto afterInference = time1.tv_sec * 1000 + time1.tv_usec / 1000;
        if(!isTracking)
            sprintf(fps, "FPS: %d", 120);
        else
            sprintf(fps, "FPS: %d", (int)(1000.0 / float(afterInference - beforeInference)));
        cv::putText(dets.img, fps, cv::Point(16, 32), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 2);

        cv::imshow("Camera FPS", dets.img);
        if (cv::waitKey(1) == 'q') // 延时1毫秒,按q键退出/Press q to exit
            break;
        frames++;

        if(!isTracking) {
            if (mouseData.clicked)
            {
                // cout << mouseData.x << " " << mouseData.y << endl;
                for (const auto& rect: dets.dets.results)
                {
                    if (pointInRectangle(mouseData.x, mouseData.y, rect.box))
                    {
                        rectangle(img, cv::Point(rect.box.x, rect.box.y), cv::Point(rect.box.x + rect.box.width, rect.box.y + rect.box.height), cv::Scalar(255, 0, 0), 2);
                        // trk_id = rect.trackID;
                        tracker->init(dets.img, rect.box);

                        isTracking = true;
                        // cv::imshow("Camaera FPS", img);
                        break;
                    }
                }
            }
        }

        if (frames % 120 == 0 && !isTracking)
        {
            gettimeofday(&time, nullptr);
            auto currentTime = time.tv_sec * 1000 + time.tv_usec / 1000;
            printf("120帧内平均帧率:\t %f fps/s\n", 120.0 / float(currentTime - beforeTime) * 1000.0);
            beforeTime = currentTime;
        }
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

    gettimeofday(&time, nullptr);
    auto endTime = time.tv_sec * 1000 + time.tv_usec / 1000;

    printf("Average:\t %f fps/s\n", float(frames) / float(endTime - startTime) * 1000.0);

    return 0;
}
