#ifndef RK3588_DETECT_TRACK_RESIZE_H_
#define RK3588_DETECT_TRACK_RESIZE_H_

#include "opencv2/core/mat.hpp"
#include "im2d.h"
#include "RgaUtils.h"
#include "rga.h"

class PreResize
{
public:
    PreResize();
    PreResize(int, int, int);
    void init(double, double);
    int input_height;
    int input_width;
    int input_channel;
    double fx;  // scale along x
    double fy;  // scale along y
    // init rga context
    rga_buffer_t src;
    rga_buffer_t dst;
    im_rect src_rect;
    im_rect dst_rect;

	void resize(cv::Mat &img, cv::Mat &_img);
};

#endif
