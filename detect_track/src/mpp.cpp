#include "mpp-v2.h"
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

unsigned int framecount = 0;
unsigned int width = 0,height = 0;

/*--defined in ffmpeg--*/
const AVCodec * codec;
AVCodecContext *codecCtx;
AVFormatContext * formatCtx;
AVStream * stream;
AVHWDeviceType type = AV_HWDEVICE_TYPE_DRM;
AVBufferRef *hwdevice;
AVBufferRef *hwframe;
AVHWFramesContext * hwframeCtx;
AVFrame *frame; // 封装DRM的帧
AVPacket * packet; // 发送的包
long extra_data_size = 10000000;
uint8_t* cExtradata = NULL; // 数据头

AVPixelFormat hd_pix = AV_PIX_FMT_DRM_PRIME;
AVPixelFormat sw_pix = AV_PIX_FMT_YUV420P;

/*--defined in mpp--*/
MppBufferGroup group;
MppBufferInfo info;
MppBuffer buffer;
MppBuffer commitBuffer;
MppFrame mppframe;
MppPacket mppPacket;

MppApi *mppApi;
MppCtx mppCtx;
MppEncCfg cfg;
MppTask task;
MppMeta meta;

unsigned int hor_stride = 0,ver_stride = 0; 

unsigned int yuv_width = 0,yuv_height = 0;
unsigned int yuv_hor_stride = 0,yuv_ver_stride = 0; 

unsigned int image_size = 0;

void rkmpp_release_packet(void *opaque, uint8_t *data){
    RKMPPPacketContext *pkt_ctx = (RKMPPPacketContext *)opaque;
    mpp_packet_deinit(&pkt_ctx->packet);
    av_buffer_unref(&pkt_ctx->encoder_ref);
    av_free(pkt_ctx);
}

int init_encoder(Command & cmd){
    int res = 0;
    // avdevice_register_all();
    avformat_network_init();

    codec = avcodec_find_encoder_by_name("h264_rkmpp");
    if(!codec){
        fprintf(stderr, "can not find h264_rkmpp encoder!\n");
        return -1;
    }
    // 创建编码器上下文
    codecCtx = avcodec_alloc_context3(codec);
    if(!codecCtx){
        fprintf(stderr, "can not create codec Context of h264_rkmpp!\n");
        return -1;
    }
    
    res = av_hwdevice_ctx_create(&hwdevice,type,"/dev/dri/card0",0,0);
    if(res < 0){
        fprintf(stderr, "create hdwave device context failed!\n");
        return res;
    }

    codecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    codecCtx->codec_id = codec->id;
    codecCtx->codec = codec;
    codecCtx->bit_rate = 1024*1024*8;
    codecCtx->codec_type = AVMEDIA_TYPE_VIDEO; //解码类型
    codecCtx->width = width;  // 宽
    codecCtx->height = height; // 高
    // codecCtx->channels = 0;
    codecCtx->time_base = (AVRational){1,cmd.getFps()}; // 每帧的时间
    codecCtx->framerate = (AVRational){cmd.getFps(),1}; // 帧率
    codecCtx->pix_fmt = hd_pix; //AV_PIX_FMT_DRM_PRIME
    codecCtx->gop_size = 12; // 每组多少帧
    codecCtx->max_b_frames = 0; // b帧最大间隔

    hwframe = av_hwframe_ctx_alloc(hwdevice);
    if(!hwframe){
        fprintf(stderr, "create hdwave frame context failed!\n");
        return -1;
    }
    hwframeCtx = (AVHWFramesContext *)(hwframe->data);
    hwframeCtx->format    = hd_pix;
    hwframeCtx->sw_format = sw_pix;
    hwframeCtx->width     = width;
    hwframeCtx->height    = height;
    /**
     *  帧池，会预分配，后面创建与硬件关联的帧时，会从该池后面获取相应的帧
     *  initial_pool_size与pool 至少要有一个不为空
    */
    // hwframeCtx->initial_pool_size = 20;
    hwframeCtx->pool = av_buffer_pool_init(20*sizeof(AVFrame),NULL);
    res = av_hwframe_ctx_init(hwframe);
    if(res < 0){
        fprintf(stderr, "init hd frame context failed!\n");
        return res;
    }
    codecCtx->hw_frames_ctx = hwframe;
    codecCtx->hw_device_ctx = hwdevice;

    if(!strcmp(cmd.getProtocol().c_str(),"rtsp")){
        // rtsp协议
        res = avformat_alloc_output_context2(&formatCtx,NULL,"rtsp",cmd.getUrl().c_str());
    }else{
        // rtmp协议
        res = avformat_alloc_output_context2(&formatCtx,NULL,"flv",cmd.getUrl().c_str());
    }
    if(res < 0){
        fprintf(stderr, "create output context failed!");
        return res;
    }

    stream = avformat_new_stream(formatCtx, codec);
    if(!stream){
        fprintf(stderr, "create stream failed!");
        return -1;
    }
    stream->time_base = (AVRational){1, cmd.getFps()}; // 设置帧率
    stream->id = formatCtx->nb_streams - 1; // 设置流的索引

    res = avcodec_parameters_from_context(stream->codecpar,codecCtx);
    if(res < 0){
        fprintf(stderr, "copy parameters to stream failed!\n");
        return -1;
    }

    // 打开输出IO RTSP不需要打开，RTMP需要打开
    if(!strcmp(cmd.getProtocol().c_str(),"rtmp")){
        res = avio_open2(&formatCtx->pb, cmd.getUrl().c_str(), AVIO_FLAG_WRITE,NULL,NULL);
        if(res < 0){
            fprintf(stderr, "avio open failed!\n");
            return -1;
        }
    }
    // 写入头信息   
    AVDictionary *opt = NULL;
    if(cmd.getProtocol() != "rtsp"){
        av_dict_set(&opt, "rtsp_transport", cmd.getTransProtocol().c_str(), 0);
        av_dict_set(&opt, "muxdelay", "0.1", 0);
    }
	res = avformat_write_header(formatCtx, &opt);
	if(res < 0){
        fprintf(stderr, "avformat write header failed!\n");
        return -1;
	}
    av_dump_format(formatCtx, 0, cmd.getUrl().c_str(), 1);
    return res;
}

MPP_RET init_mpp(){
    MPP_RET res = MPP_OK;
    res = mpp_create(&mppCtx,&mppApi);
    res = mpp_init(mppCtx,MPP_CTX_ENC,MPP_VIDEO_CodingAVC);
    res = mpp_enc_cfg_init(&cfg);
    res = mppApi->control(mppCtx,MPP_ENC_GET_CFG,cfg);

    mpp_enc_cfg_set_s32(cfg, "prep:width", width);
    mpp_enc_cfg_set_s32(cfg, "prep:height", height);
    mpp_enc_cfg_set_s32(cfg, "prep:hor_stride", hor_stride);
    mpp_enc_cfg_set_s32(cfg, "prep:ver_stride", ver_stride);
    mpp_enc_cfg_set_s32(cfg, "prep:format", MPP_FMT_YUV420P);

    mpp_enc_cfg_set_s32(cfg, "rc:quality", MPP_ENC_RC_QUALITY_BEST);
    mpp_enc_cfg_set_s32(cfg, "rc:mode", MPP_ENC_RC_MODE_VBR);

    /* fix input / output frame rate */
    mpp_enc_cfg_set_s32(cfg, "rc:fps_in_flex", 0);
    mpp_enc_cfg_set_s32(cfg, "rc:fps_in_num", 60);
    mpp_enc_cfg_set_s32(cfg, "rc:fps_in_denorm",1);
    mpp_enc_cfg_set_s32(cfg, "rc:fps_out_flex", 0);
    mpp_enc_cfg_set_s32(cfg, "rc:fps_out_num", 60);
    mpp_enc_cfg_set_s32(cfg, "rc:fps_out_denorm", 1);

    /* drop frame or not when bitrate overflow */
    mpp_enc_cfg_set_u32(cfg, "rc:drop_mode", MPP_ENC_RC_DROP_FRM_DISABLED);
    mpp_enc_cfg_set_u32(cfg, "rc:drop_thd", 20);        /* 20% of max bps */
    mpp_enc_cfg_set_u32(cfg, "rc:drop_gap", 1);         /* Do not continuous drop frame */

    /* setup bitrate for different rc_mode */
    mpp_enc_cfg_set_s32(cfg, "rc:bps_max", 10*1024*1024 * 17 / 16);
    mpp_enc_cfg_set_s32(cfg, "rc:bps_min", 10*1024*1024 * 15 / 16);

    mpp_enc_cfg_set_s32(cfg,"split:mode", MPP_ENC_SPLIT_NONE);
    // mpp_enc_cfg_set_s32(cfg,"split_arg", 0);
    // mpp_enc_cfg_set_s32(cfg,"split_out", 0);

    mpp_enc_cfg_set_s32(cfg, "h264:profile", 100);
    mpp_enc_cfg_set_s32(cfg, "h264:level", 42);
    mpp_enc_cfg_set_s32(cfg, "h264:cabac_en", 1);
    mpp_enc_cfg_set_s32(cfg, "h264:cabac_idc", 0);
    mpp_enc_cfg_set_s32(cfg, "h264:trans8x8", 1);

    mppApi->control(mppCtx, MPP_ENC_SET_CFG, cfg);

    MppPacket headpacket;
    RK_U8 enc_hdr_buf[1024];
    memset(enc_hdr_buf,0,1024);

    // 获取头数据
    mpp_packet_init(&headpacket,enc_hdr_buf,1024);

    res = mppApi->control(mppCtx, MPP_ENC_GET_HDR_SYNC, headpacket);
    void *ptr   = mpp_packet_get_pos(headpacket);
    size_t len  = mpp_packet_get_length(headpacket);

    extra_data_size = len;
    cExtradata = (uint8_t *)malloc((extra_data_size) * sizeof(uint8_t));

    memcpy(cExtradata,ptr,len); // 拷贝头数据
    mpp_buffer_group_get_external(&group,MPP_BUFFER_TYPE_DRM);
    mpp_packet_deinit(&headpacket);

    fprintf(stderr, " init mpp encoder  finished!\n");
    return res;
}

int init_data(){
    int res = 0;
    // 给packet 分配内存
    packet = av_packet_alloc();
    return res;
}

MPP_RET read_frame(cv::Mat & cvframe,void * ptr){
    RK_U32 row = 0;
    RK_U32 read_size = 0;
    RK_U8 *buf_y = (RK_U8 *)ptr;
    RK_U8 *buf_u = buf_y + hor_stride * ver_stride; // NOTE: diff from gen_yuv_image
    RK_U8 *buf_v = buf_u + hor_stride * ver_stride / 4; // NOTE: diff from gen_yuv_image
    // buf_y = cvframe.data;

    for (row = 0; row < height; row++) {
        memcpy(buf_y + row * hor_stride, cvframe.datastart + read_size, width);
        read_size += width;
    }

    for (row = 0; row < height / 2; row++) {
        memcpy(buf_u + row * hor_stride/2, cvframe.datastart + read_size, width / 2);
        read_size += width/2;
    }

    for (row = 0; row < height / 2; row++) {
        memcpy(buf_v + row * hor_stride/2,cvframe.datastart + read_size ,width/2);
        read_size += width/2;
    }
    return MPP_OK;
}

int send_packet(Command &cmd){
    int res = 0;
    packet->pts = av_rescale_q_rnd(framecount, codecCtx->time_base, stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_NEAR_INF));
    packet->dts = av_rescale_q_rnd(framecount, codecCtx->time_base, stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_NEAR_INF));
    packet->duration = av_rescale_q_rnd(packet->duration, codecCtx->time_base, stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_NEAR_INF));

     if(!(packet->flags & AV_PKT_FLAG_KEY)){
        // 在每帧非关键帧前面添加PPS SPS头信息
        /**
         * 使用h264_rkmpp编码器时，rtsp/rtmp协议都需要添加PPS
         * libx264只需要在rtsp协议时添加PPS,rtmp会自动加上
        */
        int packet_data_size = packet->size;
        u_char frame_data[packet_data_size];
        memcpy(frame_data,packet->data,packet->size);
        memcpy(packet->data,cExtradata,extra_data_size);
        memcpy(packet->data+extra_data_size,frame_data,packet_data_size);
        packet->size = packet_data_size + extra_data_size;
    }
    // 通过创建输出流的format 输出数据包
    framecount++;
    res = av_interleaved_write_frame(formatCtx, packet);
    if (res < 0){
        fprintf(stderr, "send packet error!");
        return -1;
    }
    return 0;
}
/**
 * 将opencv的帧转换成drm数据帧
 * 并送入mpi解码，转换成AvPacket
*/
MPP_RET convert_cvframe_to_drm(cv::Mat &cvframe,AVFrame *& avframe,Command &cmd){
    MPP_RET res = MPP_OK;
    res = mpp_buffer_get(NULL,&buffer,image_size);
    if(res != MPP_OK){
        return res;
    }
    info.fd = mpp_buffer_get_fd(buffer);
    info.ptr = mpp_buffer_get_ptr(buffer);
    info.index = framecount;
    info.size = image_size;
    info.type = MPP_BUFFER_TYPE_DRM;
    // 将数据读入buffer
    read_frame(cvframe,info.ptr);

    res = mpp_buffer_commit(group,&info);
    if(res != MPP_OK){
        return res;
    }

    res = mpp_buffer_get(group,&commitBuffer,image_size);
    if(res != MPP_OK){
        return res;
    }

    mpp_frame_init(&mppframe);
    mpp_frame_set_width(mppframe,width);
    mpp_frame_set_height(mppframe,height);
    mpp_frame_set_hor_stride(mppframe,yuv_hor_stride);
    mpp_frame_set_ver_stride(mppframe,ver_stride);
    mpp_frame_set_buf_size(mppframe,image_size);
    mpp_frame_set_buffer(mppframe,commitBuffer);
    /**
     * 使用mpp可以使用 YUV格式的数据外 还能使用RGB格式的数据
    */
    mpp_frame_set_fmt(mppframe,MPP_FMT_YUV420P); // YUV420SP == NV12 
    mpp_frame_set_eos(mppframe,0);

    mpp_packet_init_with_buffer(&mppPacket, commitBuffer);
    mpp_packet_set_length(mppPacket, 0);
    // set frame
    res = mppApi->poll(mppCtx, MPP_PORT_INPUT, MPP_POLL_BLOCK);
    res = mppApi->dequeue(mppCtx, MPP_PORT_INPUT, &task);
    res = mpp_task_meta_set_packet(task,KEY_OUTPUT_PACKET,mppPacket);
    res = mpp_task_meta_set_frame(task, KEY_INPUT_FRAME, mppframe);
    res = mppApi->enqueue(mppCtx, MPP_PORT_INPUT, task);

    // get packet
    res = mppApi->poll(mppCtx, MPP_PORT_OUTPUT, MPP_POLL_BLOCK);
    res = mppApi->dequeue(mppCtx, MPP_PORT_OUTPUT, &task);
    res = mpp_task_meta_get_packet(task, KEY_OUTPUT_PACKET, &mppPacket);
    res = mppApi->enqueue(mppCtx, MPP_PORT_OUTPUT, task);

    int is_eoi = mpp_packet_is_eoi(mppPacket);

    if (mppPacket) {
        RKMPPPacketContext *pkt_ctx = (RKMPPPacketContext *)av_mallocz(sizeof(*pkt_ctx));
        pkt_ctx->packet = mppPacket;
        int keyframe = 0;
        // TODO: outside need fd from mppbuffer?
        packet->data = (uint8_t *)mpp_packet_get_data(mppPacket);
        packet->size = mpp_packet_get_length(mppPacket);

        packet->buf = av_buffer_create((uint8_t*)packet->data, packet->size,
            rkmpp_release_packet, pkt_ctx, AV_BUFFER_FLAG_READONLY);
        packet->pts = mpp_packet_get_pts(mppPacket);
        packet->dts = mpp_packet_get_dts(mppPacket);

        if (packet->pts <= 0)
            packet->pts = packet->dts;
        if (packet->dts <= 0)
            packet->dts = packet->pts;
        meta = mpp_packet_get_meta(mppPacket);
        if (meta)
            mpp_meta_get_s32(meta, KEY_OUTPUT_INTRA, &keyframe);
        if (keyframe){
            packet->flags |= AV_PKT_FLAG_KEY;
        }
    }

    send_packet(cmd);
    return res;
}

int transfer_frame(cv::Mat &cvframe,Command &cmd){
    int res = 0;
    
    convert_cvframe_to_drm(cvframe, frame, cmd);

    if(buffer != NULL){
        mpp_buffer_put(buffer); // 清空buffer
        buffer = NULL;
    }
    if(commitBuffer != NULL){
        mpp_buffer_put(commitBuffer); // 清空buffer
        commitBuffer = NULL;
    }
    /**
     * TODO:为什么这里不需要deinit
     * 还是说编码完 对于的buffer自动释放？
    */
    // mpp_packet_deinit(&mppPacket); 
    mpp_buffer_group_clear(group);
    mpp_frame_deinit(&mppframe);
    return 0;
}

void destory(){
    // cout << "释放回收资源：" << endl;
    mpp_buffer_group_put(group);
	// fclose(wf);
	if(formatCtx){
        // avformat_close_input(&avFormatCtx);
        avio_close(formatCtx->pb);
        avformat_free_context(formatCtx);
        formatCtx = 0;
        // cout << "avformat_free_context(formatCtx)" << endl;
    }
    if(packet){
        av_packet_unref(packet);
        packet = NULL;
        // cout << "av_free_packet(packet)" << endl;
    }

    if(frame){
        av_frame_free(&frame);
        frame = 0;
        // cout << "av_frame_free(frame)" << endl;
    }
   
    if(codecCtx->hw_device_ctx){
        av_buffer_unref(&codecCtx->hw_device_ctx);
        // cout << "av_buffer_unref(&codecCtx->hw_device_ctx)" << endl;
    }

    if(codecCtx){
        avcodec_close(codecCtx);
        codecCtx = 0;
        // cout << "avcodec_close(codecCtx);" << endl;
    }
    if(cExtradata){
        free(cExtradata);
        // cout << "free cExtradata " << endl;
    }
    if(hwdevice){
        av_buffer_unref(&hwdevice);
        // cout << "av_buffer_unref hwdevice " << endl;
    }
    if(hwframe){
        av_buffer_unref(&hwframe);
        // cout << "av_buffer_unref encodeHwBufRef " << endl;
    }
}

cv::VideoCapture capture_init(Command &cmd) {
    cv::VideoCapture capture;
    std::string src = cmd.getSrc();
    height = cmd.getHeight();
    width = cmd.getWidth();
    if (src.length() == 1) {
        capture.open((int)(src[0] - '0'), cv::CAP_V4L2);
        capture.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
        capture.set(cv::CAP_PROP_FRAME_WIDTH, width);
        capture.set(cv::CAP_PROP_FRAME_HEIGHT, height);
        // capture.set(cv::CAP_PROP_FPS, 30);
    }
    else {
        capture.open(src.c_str(), cv::CAP_FFMPEG);
    }

    cv::Mat bgrFrame(height, width, CV_8UC3);
    cv::Mat yuvFrame;

    cv::cvtColor(bgrFrame, yuvFrame, cv::COLOR_BGR2YUV_YV12);
    cv::Size yuvSize = yuvFrame.size();
    yuv_height = yuvSize.height;
    yuv_width = yuvSize.width;
    yuv_hor_stride = MPP_ALIGN(yuv_width, 16); // 1920
    yuv_ver_stride = MPP_ALIGN(yuv_height, 16); // 1632

    hor_stride = MPP_ALIGN(width, 16); // 1920
    ver_stride = MPP_ALIGN(height, 16); // 1088    

    image_size = sizeof(unsigned char) * hor_stride *  ver_stride * 3 / 2;

    init_encoder(cmd);
    init_data();
    init_mpp();
    
    return capture;
}