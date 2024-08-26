#ifndef RK3588_DETECT_TRACK_MPP_H_
#define RK3588_DETECT_TRACK_MPP_H_

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "rk_mpi.h"
#include "mpp_env.h"
#include "mpp_mem.h"
#include "mpp_log.h"
#include "mpp_time.h"
#include "mpp_common.h"

typedef struct
{
    MppCodingType   type;
    RK_U32          width;
    RK_U32          height;
    MppFrameFormat  format;
 
    RK_U32          num_frames;
} MpiEncTestCmd;
 
typedef struct
{
    //global flow control flag
    RK_U32 frm_eos;
    RK_U32 pkt_eos;
    RK_U32 frame_count;
    RK_U64 stream_size;
 
    //input ang output file
    FILE *fp_input;
    FILE *fp_output;
 
    //input and output
    MppBuffer frm_buf;
    MppEncSeiMode sei_mode;
 
    //base flow context
    MppCtx ctx;
    MppApi *mpi;
    MppEncPrepCfg prep_cfg;
    MppEncRcCfg rc_cfg;
    MppEncCodecCfg codec_cfg;
 
    //paramter for resource malloc
    RK_U32 width;
    RK_U32 height;
    RK_U32 hor_stride; //horizontal stride
    RK_U32 ver_stride; //vertical stride
    MppFrameFormat fmt;
    MppCodingType type;
    RK_U32 num_frames;
 
    //resources
    size_t frame_size;
    //NOTE: packet buffer may overflow
    size_t packet_size;
 
    //rate control runtime parameter
    RK_S32 gop;
    RK_S32 fps;
    RK_S32 bps;
} MpiEncTestData;

//------------------------------------------------------------------------------
MpiEncTestData encoder_params;
MpiEncTestData *encoder_params_ptr = &encoder_params;
//
MppApi *mpi;
MppCtx ctx;
//
bool first_frame_flg = true;
//------------------------------------------------------------------------------
//功能：MPP上下文初始化
//说明：根据MpiEncTestCmd参数设置MpiEncTestData参数
MPP_RET test_ctx_init(MpiEncTestData **data, MpiEncTestCmd *cmd)
{
    MpiEncTestData *p = NULL;
    MPP_RET ret = MPP_OK;
 
    if (!data || !cmd)
    {
        mpp_err_f("invalid input data %p cmd %p\n", data, cmd);
        return MPP_ERR_NULL_PTR;
    }
 
    p = mpp_calloc(MpiEncTestData, 1);
    if (!p)
    {
        mpp_err_f("create MpiEncTestData failed\n");
        ret = MPP_ERR_MALLOC;
        goto RET;
    }
 
    //get paramter from cmd
    p->width        = cmd->width;
    p->height       = cmd->height;
    p->hor_stride   = MPP_ALIGN(cmd->width, 16);
    p->ver_stride   = MPP_ALIGN(cmd->height, 16);
    p->fmt          = cmd->format;
    p->type         = cmd->type;
    p->num_frames   = cmd->num_frames;
    p->frame_size   = p->hor_stride * p->ver_stride * 3 / 2;
    p->packet_size  = p->width * p->height;
 
RET:
    *data = p;
    return ret;
}


//------------------------------------------------------------------------------
//功能：设置MPP编码器参数
//说明：1-输入控制配置；2-码率控制配置；3-协议控制配置；4-SEI模式配置
//------------------------------------------------------------------------------
MPP_RET test_mpp_setup(MpiEncTestData *p)
{
    MPP_RET ret;
    MppApi *mpi;
    MppCtx ctx;
    MppEncCodecCfg *codec_cfg;
    MppEncPrepCfg *prep_cfg;
    MppEncRcCfg *rc_cfg;
 
    if (NULL == p)
    {
        return MPP_ERR_NULL_PTR;
    }
 
    mpi = p->mpi;
    ctx = p->ctx;
    codec_cfg = &p->codec_cfg;
    prep_cfg = &p->prep_cfg;
    rc_cfg = &p->rc_cfg;
 
    p->fps = 30;
    p->gop = 60;
    //p->bps = p->width * p->height / 5 * p->fps;
    p->bps = 4096*1024;
 
    //1--输入控制配置
    prep_cfg->change        = MPP_ENC_PREP_CFG_CHANGE_INPUT | MPP_ENC_PREP_CFG_CHANGE_ROTATION | MPP_ENC_PREP_CFG_CHANGE_FORMAT;
    prep_cfg->width         = p->width;
    prep_cfg->height        = p->height;
    prep_cfg->hor_stride    = p->hor_stride;
    prep_cfg->ver_stride    = p->ver_stride;
    prep_cfg->format        = p->fmt;
    prep_cfg->rotation      = MPP_ENC_ROT_0;
    ret = mpi->control(ctx, MPP_ENC_SET_PREP_CFG, prep_cfg);
    if (ret)
    {
        mpp_err("mpi control enc set prep cfg failed ret %d\n", ret);
        goto RET;
    }
 
    //2--码率控制配置
    rc_cfg->change  = MPP_ENC_RC_CFG_CHANGE_ALL;
    rc_cfg->rc_mode = MPP_ENC_RC_MODE_VBR;
    rc_cfg->quality = MPP_ENC_RC_QUALITY_CQP;
 
    if (rc_cfg->rc_mode == MPP_ENC_RC_MODE_CBR)
    {
        //constant bitrate has very small bps range of 1/16 bps
        rc_cfg->bps_target   = p->bps;
        rc_cfg->bps_max      = p->bps * 17 / 16;
        rc_cfg->bps_min      = p->bps * 15 / 16;
    }
    else if (rc_cfg->rc_mode ==  MPP_ENC_RC_MODE_VBR)
    {
        if (rc_cfg->quality == MPP_ENC_RC_QUALITY_CQP)
        {
            //constant QP does not have bps
            //rc_cfg->bps_target   = -1;
            //rc_cfg->bps_max      = -1;
            //rc_cfg->bps_min      = -1;
            rc_cfg->bps_target   = p->bps;
            rc_cfg->bps_max      = p->bps * 17 / 16;
            rc_cfg->bps_min      = p->bps * 1 / 16;
        }
        else
        {
            //variable bitrate has large bps range
            rc_cfg->bps_target   = p->bps;
            rc_cfg->bps_max      = p->bps * 17 / 16;
            rc_cfg->bps_min      = p->bps * 1 / 16;
        }
    }
 
    //fix input / output frame rate
    rc_cfg->fps_in_flex      = 0;
    rc_cfg->fps_in_num       = p->fps;
    rc_cfg->fps_in_denom    = 1;
    rc_cfg->fps_out_flex     = 0;
    rc_cfg->fps_out_num      = p->fps;
    rc_cfg->fps_out_denom   = 1;
 
    rc_cfg->gop              = p->gop;
    rc_cfg->skip_cnt         = 0;
 
    mpp_log("mpi_enc_test bps %d fps %d gop %d\n", rc_cfg->bps_target, rc_cfg->fps_out_num, rc_cfg->gop);
    ret = mpi->control(ctx, MPP_ENC_SET_RC_CFG, rc_cfg);
    if (ret)
    {
        mpp_err("mpi control enc set rc cfg failed ret %d\n", ret);
        goto RET;
    }
 
    //3--协议控制配置
    codec_cfg->coding = p->type;
    codec_cfg->h264.change = MPP_ENC_H264_CFG_CHANGE_PROFILE | MPP_ENC_H264_CFG_CHANGE_ENTROPY | MPP_ENC_H264_CFG_CHANGE_TRANS_8x8;
 
    //66  - Baseline profile
    //77  - Main profile
    //100 - High profile
    codec_cfg->h264.profile = 77;
    /*
    * H.264 level_idc parameter
    * 10 / 11 / 12 / 13    - qcif@15fps / cif@7.5fps / cif@15fps / cif@30fps
    * 20 / 21 / 22         - cif@30fps / half-D1@@25fps / D1@12.5fps
    * 30 / 31 / 32         - D1@25fps / 720p@30fps / 720p@60fps
    * 40 / 41 / 42         - 1080p@30fps / 1080p@30fps / 1080p@60fps
    * 50 / 51 / 52         - 4K@30fps
    */
    codec_cfg->h264.level    = 41;
    codec_cfg->h264.entropy_coding_mode  = 1;
    codec_cfg->h264.cabac_init_idc  = 0;
    //codec_cfg->h264.qp_min = 0;
    //codec_cfg->h264.qp_max = 50;
    //codec_cfg->h264.transform8x8_mode = 0;
    ret = mpi->control(ctx, MPP_ENC_SET_CODEC_CFG, codec_cfg);
    if (ret)
    {
        mpp_err("mpi control enc set codec cfg failed ret %d\n", ret);
        goto RET;
    }
    //4--SEI模式配置
    p->sei_mode = MPP_ENC_SEI_MODE_ONE_FRAME;
    ret = mpi->control(ctx, MPP_ENC_SET_SEI_CFG, &p->sei_mode);
    if (ret)
    {
        mpp_err("mpi control enc set sei cfg failed ret %d\n", ret);
        goto RET;
    }
 
RET:
    return ret;
}

//------------------------------------------------------------------------------
//功能：将YUV420视频帧数据填充到MPP buffer
//说明：使用16字节对齐，MPP可以实现零拷贝，提高效率
//------------------------------------------------------------------------------
void read_yuv_buffer(RK_U8 *buf, cv::Mat &yuvImg, RK_U32 width, RK_U32 height)
{
    RK_U8 *buf_y = buf;
    RK_U8 *buf_u = buf + MPP_ALIGN(width, 16) * MPP_ALIGN(height, 16);
    RK_U8 *buf_v = buf_u + MPP_ALIGN(width, 16) * MPP_ALIGN(height, 16) / 4;
    //
    RK_U8 *yuvImg_y = yuvImg.data;
    RK_U8 *yuvImg_u = yuvImg_y + width * height;
    RK_U8 *yuvImg_v = yuvImg_u + width * height / 4;
    //
    memcpy(buf_y, yuvImg_y, width * height);
    memcpy(buf_u, yuvImg_u, width * height / 4);
    memcpy(buf_v, yuvImg_v, width * height / 4);
}

//------------------------------------------------------------------------------
//功能：MPP执行编码
//------------------------------------------------------------------------------
MPP_RET test_mpp_run_yuv(cv::Mat yuvImg, MppApi *mpi, MppCtx &ctx, unsigned char * &H264_buf, int &length)
{
    MpiEncTestData *p = encoder_params_ptr;
    MPP_RET ret;
 
    MppFrame frame = NULL;
    MppPacket packet = NULL;
    void *buf = mpp_buffer_get_ptr(p->frm_buf);
    read_yuv_buffer((RK_U8*)buf, yuvImg, p->width, p->height);
    ret = mpp_frame_init(&frame);
    if (ret)
    {
        mpp_err_f("mpp_frame_init failed\n");
        goto RET;
    }
    //
    mpp_frame_set_width(frame, p->width);
    mpp_frame_set_height(frame, p->height);
    mpp_frame_set_hor_stride(frame, p->hor_stride);
    mpp_frame_set_ver_stride(frame, p->ver_stride);
    mpp_frame_set_fmt(frame, p->fmt);
    mpp_frame_set_buffer(frame, p->frm_buf);
    mpp_frame_set_eos(frame, p->frm_eos);
 
    ret = mpi->encode_put_frame(ctx, frame);
    if (ret)
    {
        mpp_err("mpp encode put frame failed\n");
        goto RET;
    }
    ret = mpi->encode_get_packet(ctx, &packet);
    if (ret)
    {
        mpp_err("mpp encode get packet failed\n");
        goto RET;
    }
    if (packet)
    {
        void *ptr   = mpp_packet_get_pos(packet);
        size_t len  = mpp_packet_get_length(packet);
        p->pkt_eos = mpp_packet_get_eos(packet);
        //
        H264_buf = new unsigned char[len];
        memcpy(H264_buf, ptr, len);
        length = len;
        mpp_packet_deinit(&packet);
        p->stream_size += len;
        p->frame_count++;
        if (p->pkt_eos)
        {
            mpp_log("found last packet\n");
            mpp_assert(p->frm_eos);
        }
    }
RET:
    return ret;
}

//------------------------------------------------------------------------------
//功能：初始化MPP编码器
//------------------------------------------------------------------------------
MpiEncTestData *test_mpp_run_yuv_init(MpiEncTestData *p, int width , int height, unsigned char * &SPS_buf, int &SPS_length)
{
    MPP_RET ret;
    //
    MpiEncTestCmd cmd;
    cmd.width = width;
    cmd.height = height;
    cmd.type = MPP_VIDEO_CodingAVC;
    cmd.format = MPP_FMT_YUV420P;
    cmd.num_frames = 0;
    ret = test_ctx_init(&p, &cmd);
    if (ret)
    {
        mpp_err_f("test data init failed ret %d\n", ret);
        goto MPP_TEST_OUT;
    }
    //
    mpp_log("p->frame_size = %d----------------\n", p->frame_size);
    ret = mpp_buffer_get(NULL, &p->frm_buf, p->frame_size);
    if (ret)
    {
        mpp_err_f("failed to get buffer for input frame ret %d\n", ret);
        goto MPP_TEST_OUT;
    }
    //
    mpp_log("mpi_enc_test encoder test start w %d h %d type %d\n",p->width, p->height, p->type);
    //encoder demo
    ret = mpp_create(&p->ctx, &p->mpi);
    if (ret)
    {
        mpp_err("mpp_create failed ret %d\n", ret);
        goto MPP_TEST_OUT;
    }
    //
    ret = mpp_init(p->ctx, MPP_CTX_ENC, p->type);
    if (ret)
    {
        mpp_err("mpp_init failed ret %d\n", ret);
        goto MPP_TEST_OUT;
    }
    //
    ret = test_mpp_setup(p);
    if (ret)
    {
        mpp_err_f("test mpp setup failed ret %d\n", ret);
        goto MPP_TEST_OUT;
    }
 
    mpi = p->mpi;
    ctx = p->ctx;
    //
    if (p->type == MPP_VIDEO_CodingHEVC)
    {
        MppPacket packet = NULL;
        ret = mpi->control(ctx, MPP_ENC_GET_EXTRA_INFO, &packet);
        if (ret)
        {
            mpp_err("mpi control enc get extra info failed\n");
        }
        //get and write sps/pps for H.264
        if (packet)
        {
            void *ptr    = mpp_packet_get_pos(packet);
            size_t len    = mpp_packet_get_length(packet);
            SPS_buf = new unsigned char[len];
            memcpy(SPS_buf, ptr, len);
            SPS_length = len;
            packet = NULL;
        }
    }
    return p;
MPP_TEST_OUT:
    return p;
}

//------------------------------------------------------------------------------
//功能：将YUV420格式图像帧编码为H264数据包
//------------------------------------------------------------------------------
void YuvtoH264(int width, int height, cv::Mat yuv_frame, unsigned char* (&encode_buf), int &encode_length)
{
    unsigned char *H264_buf = NULL;
    int H264_buf_length = 0;
    unsigned char *SPS_buf = NULL;
    int SPS_length = 0;
    //
    if(first_frame_flg == true)
    {
        encoder_params_ptr = test_mpp_run_yuv_init(encoder_params_ptr, width, height, SPS_buf, SPS_length);
        //SPS数据，可以不使用
//        test_mpp_run_yuv(yuv_frame, mpi, ctx, H264_buf, length);
//        encode_buf = new unsigned char[SPS_length + length];
//        memcpy(encode_buf, SPS_buf, SPS_length);
//        memcpy(encode_buf + SPS_length, H264_buf, length);
//        encode_length = length + SPS_length;
 
        test_mpp_run_yuv(yuv_frame, mpi, ctx, H264_buf, H264_buf_length);
        encode_buf = new unsigned char[H264_buf_length];
        memcpy(encode_buf, H264_buf, H264_buf_length);
        encode_length = H264_buf_length;
 
        first_frame_flg = false;
        delete H264_buf;
        delete SPS_buf;
        printf("first_frame! \n");
        printf("SPS length: %d! \n", SPS_length);
        printf("frame length: %d! \n", H264_buf_length);
    }
    else
    {
        test_mpp_run_yuv(yuv_frame, mpi, ctx, H264_buf, H264_buf_length);
        encode_buf = new unsigned char[H264_buf_length];
        memcpy(encode_buf, H264_buf, H264_buf_length);
        encode_length = H264_buf_length;
        printf("frame length: %d! \n", H264_buf_length);
        delete H264_buf;
    }
}

#endif