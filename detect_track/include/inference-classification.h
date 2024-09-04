#ifndef RKNN_INCLUDE_INFERENCE_H_
#define RKNN_INCLUDE_INFERENCE_H_

//C system headers

//C++ standard library headers

//other libraries' headers
#include "rknn_api.h"
#include "opencv2/core/mat.hpp"

//project headers


typedef struct {
    rknn_context rknn_ctx;
    rknn_input_output_num io_num;
    rknn_tensor_attr* input_attrs;
    rknn_tensor_attr* output_attrs;
    int model_channel;
    int model_width;
    int model_height;
    bool is_quant;
} rknn_app_context_t;

int init_model(const char* model_path, rknn_app_context_t* app_ctx);

int release_model(rknn_app_context_t* app_ctx);

int inference_model(rknn_app_context_t* app_ctx, const cv::Mat& input, rknn_output **outputs);

#endif //RKNN_INCLUDE_INIT_MODEL_H_