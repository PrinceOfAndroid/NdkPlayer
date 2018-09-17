//
// Created by PrinceOfAndroid on 2018/9/12 0012.
//

#include <libavutil/imgutils.h>
#include "VideoChannel.h"


void *decode_task(void *args) {
    VideoChannel *videoChannel = static_cast<VideoChannel *>(args);
    videoChannel->decode();
    return 0;
}

void *render_task(void *args) {
    VideoChannel *videoChannel = static_cast<VideoChannel *>(args);
    videoChannel->render();
    return 0;
}

VideoChannel::VideoChannel(int id, AVCodecContext *avCodecContext) : BaseChannel(id,
                                                                                 avCodecContext) {

}


//析构方法 进行释放
VideoChannel::~VideoChannel(int id, AVCodecContext *avCodecContext) {
    frames.setReleaseCallback(releaseAvFrame);
}

void VideoChannel::play() {
    isPlaying = 1;
    pthread_create(&pid_decode, 0, decode_task, this);
}

void VideoChannel::decode() {
    AVPacket *packet = 0;
    while (isPlaying) {
        //取出来一个数据包
        int ret = packets.pop(packet);
        if (!isPlaying) {
            break;
        }
        //取出失败
        if (!ret) {
            continue;
        }
        ret = avcodec_send_packet(avCodecContext, packet);
        //重试
        if (ret != 0) {
            break;
        }
        //代表了一个图像
        AVFrame *frame = av_frame_alloc();
        //从解码器中读取 解码后的数据包 frame
        ret = avcodec_receive_frame(avCodecContext, frame);

        releaseAvPacket(&packet);
        //需要更多的数据才能进行解码
        if (ret == AVERROR(EAGAIN)) {
            continue;
        } else if (ret != 0) {
            break;
        }
        frames.push(frame);
        //再开一个线程来播放
        pthread_create(&pid_render, 0, render_task, this);
    }
    releaseAvPacket(&packet);
}

void VideoChannel::render() {
    swsContext = sws_getContext(avCodecContext->width, avCodecContext->height,
                                avCodecContext->pix_fmt,
                                avCodecContext->width, avCodecContext->height,
                                AV_PIX_FMT_RGBA,
                                SWS_BILINEAR, 0, 0, 0);
    AVFrame *frame = 0;
    uint8_t *dst_data[4];
    int dst_linesize[4];
    //申请空间
    av_image_alloc(dst_data, dst_linesize,
                   avCodecContext->width, avCodecContext->height, avCodecContext->pix_fmt, 1);
    while (isPlaying) {
        int ret = frames.pop(frame);
        if (!isPlaying) {
            break;
        }

        sws_scale(swsContext, reinterpret_cast<const uint8_t *const *>(frame->data),
                  frame->linesize, 0, avCodecContext->height, dst_data,
                  dst_linesize);
        callback(dst_data[0], dst_linesize[0], avCodecContext->width, avCodecContext->height);
        releaseAvFrame(&frame);
    }
    av_free(&dst_data[0]);
    releaseAvFrame(&frame);
}

void VideoChannel::setRenderFrameCallback(RenderFrameCallback callback) {
    this->callback = callback;
}
