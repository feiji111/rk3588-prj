#ifndef RK3588_DETECT_TRACK_MODEL_H_
#define RK3588_DETECT_TRACK_MODEL_H_

#include <string>

#include "opencv2/core/mat.hpp"

#include "common.h"
#include "rknn_api.h"
#include <opencv2/videoio.hpp>

static void dump_tensor_attr(rknn_tensor_attr *attr);
static unsigned char *load_data(FILE *fp, size_t ofst, size_t sz);
static unsigned char *load_model(const char *filename, int *model_size);
static int saveFloat(const char *file_name, float *output, int element_size);

class rkYolov5s
{
private:
    int ret;
    std::mutex mtx;
    std::string model_path;
    unsigned char *model_data;

    rknn_context ctx;
    rknn_input_output_num io_num;
    rknn_tensor_attr *input_attrs;
    rknn_tensor_attr *output_attrs;
    rknn_input inputs[1];

    int channel, width, height;
    int img_width, img_height;

    float nms_threshold, box_conf_threshold;

public:
    rkYolov5s(const std::string &model_path);
    int init(rknn_context *ctx_in, bool isChild);
    rknn_context *get_pctx();
    frameDets infer(cv::Mat &ori_img);
    ~rkYolov5s();
};

class NanoTrack_head {
private:
    int ret;
    rknn_context ctx;
    std::string model_path;
    unsigned char *model_data;
    rknn_input_output_num io_num;
    rknn_tensor_attr *input_attrs;
    rknn_tensor_attr *output_attrs;

    int channel0, width0, height0;
    int channel1, width1, height1;
    
    rknn_input inputs[2];

public:
    NanoTrack_head(const std::string& model_path);
    int init(rknn_context *ctx_in, bool isChild = false);
    rknn_context *get_pctx();
    int infer(cv::Mat img, cv::Mat target);
};

class NanoTrack_X_backbone {
private:
    int ret;
    rknn_context ctx;
    std::string model_path;
    unsigned char *model_data;
    rknn_input_output_num io_num;
    rknn_tensor_attr *input_attrs;
    rknn_tensor_attr *output_attrs;

    int channel, width, height;
    
    rknn_input inputs;

public:
    NanoTrack_head(const std::string& model_path);
    int init(rknn_context *ctx_in, bool isChild = false);
    rknn_context *get_pctx();
    int infer(cv::Mat img, cv::Mat target);
};


#endif