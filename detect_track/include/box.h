#ifndef RK3588_DETECT_TRACK_BOX_H_
#define RK3588_DETECT_TRACK_BOX_H_

#include <opencv2/core/types.hpp>

#define OBJ_NAME_MAX_SIZE 64
#define OBJ_NUMB_MAX_SIZE 128

typedef struct DetectBox {
    DetectBox(float x1=0, float y1=0, float x2=0, float y2=0, 
            float confidence=0, int classID=-1) {
        this->x1 = x1;
        this->y1 = y1;
        this->x2 = x2;
        this->y2 = y2;
        this->box = cv::Rect_<float>(x1, y1, x2 - x1, y2 - y1);
        this->confidence = confidence;
        this->classID = classID;
    }
    char name[OBJ_NAME_MAX_SIZE];
    float x1, y1, x2, y2;
    cv::Rect_<float> box;
    float confidence;
    int classID;
    int trackID;
} DetectBox;

typedef struct TrackBox {
    int id;
    // char name[OBJ_NAME_MAX_SIZE];
    cv::Rect_<float> box;
    // float x1, y1, x2, y2;
    // float confidence;
    // int classID;
} TrackBox;

#endif //RK3588_DETECT_TRACK_BOX_H_