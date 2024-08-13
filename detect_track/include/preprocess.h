#ifndef RK3588_DETECT_TRACK_PREPROCESS_H_
#define RK3588_DETECT_TRACK_PREPROCESS_H_

#include "opencv2/core/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

#include "postprocess.h"
#include "im2d.h"
#include "rga.h"

void letterbox(const cv::Mat &image, cv::Mat &padded_image, BOX_RECT &pads, const float scale, const cv::Size &target_size, const cv::Scalar &pad_color = cv::Scalar(128, 128, 128));

int resize_rga(rga_buffer_t &src, rga_buffer_t &dst, const cv::Mat &image, cv::Mat &resized_image, const cv::Size &target_size);

#endif //RK3588_DETECT_TRACK_PREPROCESS_H_
