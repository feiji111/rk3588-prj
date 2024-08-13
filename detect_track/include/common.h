#ifndef RK3588_DETECT_TRACK_COMMON_H_
#define RK3588_DETECT_TRACK_COMMON_H_

#include <vector>
#include <opencv2/core/mat.hpp>

#include "box.h"

// weights
#define NET_INPUTHEIGHT 640
#define NET_INPUTWIDTH 640
#define NET_INPUTCHANNEL 3

#define OBJ_CLASS_NUM     80

// threshold
#define NMS_THRESH        0.45
#define BOX_THRESH        0.25

typedef struct _detect_result_group_t
{
    int id;
    int count;
    std::vector<DetectBox> results;
} detect_result_group_t;

typedef struct {
    int left;
    int right;
    int top;
    int bottom;
    float scale;
} letterbox_t;

typedef struct frameDets{
    frameDets() {};
    frameDets(cv::Mat img, detect_result_group_t dets) : img(img), dets(dets) {};
    cv::Mat img;
    detect_result_group_t dets;
} frameDets;

#endif //RK3588_DETECT_TRACK_COMMON_H_