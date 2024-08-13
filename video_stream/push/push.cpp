#include <iostream>
#include <opencv2/opencv.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

int main(int argc, char *argv[]) {
    // OpenCV 初始化
    char* camera = argv[1];
    cv::VideoCapture cap;

    cap.open((int)(camera[0] - '0'), cv::CAP_V4L2);
    if (!cap.isOpened()) {
        std::cerr << "Could not open camera" << std::endl;
        return -1;
    }

    // 设置摄像头分辨率
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('Y', 'U', 'Y', 'V'));

    // FFmpeg 初始化
    // av_register_all();
    avformat_network_init();

    // 创建输出格式上下文
    AVFormatContext *fmt_ctx = NULL;
    const char *output_url = "rtsp://10.42.0.1:8554/mystream";
    int ret = avformat_alloc_output_context2(&fmt_ctx, NULL, "rtsp", output_url);
    if (ret < 0) {
        std::cerr << "Could not allocate output context" << std::endl;
        return -1;
    }

    // 查找视频编码器
    const AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        std::cerr << "Codec not found" << std::endl;
        return -1;
    }

    // 创建视频流
    AVStream *video_st = avformat_new_stream(fmt_ctx, codec);
    if (!video_st) {
        std::cerr << "Could not allocate stream" << std::endl;
        return -1;
    }
    video_st->id = fmt_ctx->nb_streams - 1;

    // 创建编码器上下文
    AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx) {
        std::cerr << "Could not allocate video codec context" << std::endl;
        return -1;
    }

    // 设置编码器参数
    codec_ctx->bit_rate = 400000;
    codec_ctx->width = 640;
    codec_ctx->height = 480;
    codec_ctx->time_base = (AVRational){1, 25};
    codec_ctx->framerate = (AVRational){25, 1};
    codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P; // 

    // 打开编码器
    ret = avcodec_open2(codec_ctx, codec, NULL);
    if (ret < 0) {
        std::cerr << "Could not open codec" << std::endl;
        return -1;
    }

    // 复制编码器参数到流
    avcodec_parameters_from_context(video_st->codecpar, codec_ctx);

    // 打开输出URL
    ret = avio_open(&fmt_ctx->pb, output_url, AVIO_FLAG_WRITE);
    if (ret < 0) {
        std::cerr << "Could not open output URL " << output_url << std::endl;
        return -1;
    }

    // 写文件头
    ret = avformat_write_header(fmt_ctx, NULL);
    if (ret < 0) {
        std::cerr << "Error writing header" << std::endl;
        return -1;
    }

    // 创建帧
    AVFrame *frame = av_frame_alloc();
    if (!frame) {
        std::cerr << "Could not allocate video frame" << std::endl;
        return -1;
    }
    frame->format = codec_ctx->pix_fmt;
    frame->width = codec_ctx->width;
    frame->height = codec_ctx->height;

    // 分配帧数据
    ret = av_image_alloc(frame->data, frame->linesize, codec_ctx->width, codec_ctx->height, codec_ctx->pix_fmt, 32);
    if (ret < 0) {
        std::cerr << "Could not allocate raw picture buffer" << std::endl;
        return -1;
    }

    // 创建转换上下文
    SwsContext *sws_ctx = sws_getContext(codec_ctx->width, codec_ctx->height, AV_PIX_FMT_BGR24,
                                         codec_ctx->width, codec_ctx->height, AV_PIX_FMT_YUV420P,
                                         SWS_BILINEAR, NULL, NULL, NULL);
    if (!sws_ctx) {
        std::cerr << "Could not initialize the conversion context" << std::endl;
        return -1;
    }

    // 捕获并推流
    cv::Mat img;
    AVPacket pkt;
    int frame_count = 0;
    while (true) {
        if (!cap.read(img)) {
            std::cerr << "Could not read frame" << std::endl;
            break;
        }

        // 转换图像格式
        uint8_t *srcSlice[1] = {img.data};
        int srcStride[1] = {static_cast<int>(img.step)};
        sws_scale(sws_ctx, srcSlice, srcStride, 0, codec_ctx->height, frame->data, frame->linesize);

        // 设置帧时间戳
        frame->pts = frame_count++;

        // 编码帧
        av_init_packet(&pkt);
        pkt.data = NULL;
        pkt.size = 0;

        ret = avcodec_send_frame(codec_ctx, frame);
        if (ret < 0) {
            // std::cerr << "Error sending frame to encoder: " << av_err2str(ret) << std::endl;
            break;
        }

        ret = avcodec_receive_packet(codec_ctx, &pkt);
        if (ret < 0) {
            // std::cerr << "Error receiving packet from encoder: " << av_err2str(ret) << std::endl;
            break;
        }

        // 设置包的时间戳
        pkt.stream_index = video_st->index;
        av_packet_rescale_ts(&pkt, codec_ctx->time_base, video_st->time_base);

        // 写包到输出
        ret = av_interleaved_write_frame(fmt_ctx, &pkt);
        if (ret < 0) {
            // std::cerr << "Error writing packet: " << av_err2str(ret) << std::endl;
            break;
        }
        av_packet_unref(&pkt);
    }

    // 写文件尾
    av_write_trailer(fmt_ctx);

    // 清理
    sws_freeContext(sws_ctx);
    avcodec_close(codec_ctx);
    av_free(codec_ctx);
    av_freep(&frame->data[0]);
    av_frame_free(&frame);
    avio_close(fmt_ctx->pb);
    avformat_free_context(fmt_ctx);

    return 0;
}