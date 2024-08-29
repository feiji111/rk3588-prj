#ifndef RK3588_DETECT_TRACK_MPP_H_
#define RK3588_DETECT_TRACK_MPP_H_

#include "command.h"
#include <opencv2/core.hpp>
#include <rockchip/mpp_buffer.h>
#include <rockchip/rk_mpi.h>
#include <rockchip/mpp_frame.h>
#include <rockchip/mpp_meta.h>

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libavutil/pixdesc.h>
#include <libavutil/hwcontext.h>
#include <libavutil/time.h>
}

#include <cstdio>
#include <opencv2/videoio.hpp>

#define MPP_ALIGN(x, a)         (((x)+(a)-1)&~((a)-1))

typedef struct {
    MppPacket packet;
    AVBufferRef *encoder_ref;
} RKMPPPacketContext;

void rkmpp_release_packet(void *opaque, uint8_t *data);

int init_encoder(Command &cmd);

MPP_RET init_mpp();

int init_data();

MPP_RET read_frame(cv::Mat &cvframe, void *ptr);

int send_packet(Command &cmd);

MPP_RET convert_cvframe_to_drm(cv::Mat &cvframe, AVFrame *&avframe,Command &cmd);

int transfer_frame(cv::Mat &cvframe,Command &cmd);

cv::VideoCapture capture_init(Command &cmd);

void destory();

#endif 