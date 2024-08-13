#include "preprocess.h"
#include "opencv2/imgproc.hpp"
#include <opencv2/core/mat.hpp>

cv::Mat pre_process(const cv::Mat &input, const rknn_app_context_t& app_ctx) {
    if(input.rows != app_ctx.model_height && input.cols != app_ctx.model_width) {
        cv::Mat output;
        cv::Size target_size(app_ctx.model_height, app_ctx.model_width);
        cv::resize(input, output, target_size, cv::INTER_CUBIC);
        return output;
    } else {
        return input;
    }
}