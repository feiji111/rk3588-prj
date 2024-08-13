#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <opencv2/opencv.hpp>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

int main(int argc, char* argv[]) {
    AVFormatContext* formatContext = nullptr;
    const char* url = "udp://localhost:1234";

    // 初始化 libavformat 和注册所有格式和编解码器
    // av_register_all();
    avformat_network_init();

    // 打开输入流
    if (avformat_open_input(&formatContext, url, nullptr, nullptr) != 0) {
        std::cerr << "无法打开输入流: " << url << std::endl;
        return -1;
    }

    // 检索流信息
    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
        std::cerr << "无法检索流信息" << std::endl;
        return -1;
    }

    // 打印流信息
    av_dump_format(formatContext, 0, url, 0);

    // 找到视频流
    int videoStreamIndex = -1;
    for (unsigned int i = 0; i < formatContext->nb_streams; ++i) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            break;
        }
    }

    if (videoStreamIndex == -1) {
        std::cerr << "未找到视频流" << std::endl;
        return -1;
    }

    // 获取解码器参数
    AVCodecParameters* codecParameters = formatContext->streams[videoStreamIndex]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codecParameters->codec_id);
    if (!codec) {
        std::cerr << "不支持的编解码器" << std::endl;
        return -1;
    }

    // 打开解码器
    AVCodecContext* codecContext = avcodec_alloc_context3(codec);
    if (avcodec_open2(codecContext, codec, nullptr) < 0) {
        std::cerr << "无法打开编解码器" << std::endl;
        return -1;
    }

    // 创建 OpenCV 窗口
    cv::namedWindow("Video", cv::WINDOW_NORMAL);

    // 读取帧并解码
    AVPacket packet;
    AVFrame* frame = av_frame_alloc();
    AVFrame* frameBGR = av_frame_alloc();
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_BGR24, codecContext->width, codecContext->height, 1);
    uint8_t* buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(frameBGR->data, frameBGR->linesize, buffer, AV_PIX_FMT_BGR24, codecContext->width, codecContext->height, 1);

    SwsContext* swsContext = sws_getContext(codecContext->width, codecContext->height, codecContext->pix_fmt,
                                            codecContext->width, codecContext->height, AV_PIX_FMT_BGR24,
                                            SWS_BILINEAR, nullptr, nullptr, nullptr);

    while (av_read_frame(formatContext, &packet) >= 0) {
        if (packet.stream_index == videoStreamIndex) {
            int ret = avcodec_send_packet(codecContext, &packet);
            if (ret < 0) {
                std::cerr << "发送数据包到解码器时出错" << std::endl;
                break;
            }

            while (ret >= 0) {
                ret = avcodec_receive_frame(codecContext, frame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    break;
                } else if (ret < 0) {
                    std::cerr << "从解码器接收帧时出错" << std::endl;
                    break;
                }

                // 将帧从 YUV 格式转换为 BGR 格式
                sws_scale(swsContext, frame->data, frame->linesize, 0, codecContext->height, frameBGR->data, frameBGR->linesize);

                // 创建 OpenCV 图像
                cv::Mat image(codecContext->height, codecContext->width, CV_8UC3, frameBGR->data[0]);
                std::cout << "132456" << std::endl;
                // 显示图像
                cv::imshow("Video", image);

                // 按下 'q' 键退出
                if (cv::waitKey(1) == 'q') {
                    break;
                }
            }
        }
        av_packet_unref(&packet);
    }

    // 清理
    av_frame_free(&frame);
    av_frame_free(&frameBGR);
    avcodec_free_context(&codecContext);
    avformat_close_input(&formatContext);
    avformat_network_deinit();
    av_free(buffer);
    sws_freeContext(swsContext);
    cv::destroyAllWindows();

    return 0;
}