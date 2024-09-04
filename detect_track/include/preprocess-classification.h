#ifndef RKNN_INCLUDE_PREPROCESS_H_
#define RKNN_INCLUDE_PREPROCESS_H_

#include "opencv2/core/mat.hpp"

#include "inference-classification.h"

cv::Mat pre_process(const cv::Mat& input, const rknn_app_context_t& app_ctx);

#endif