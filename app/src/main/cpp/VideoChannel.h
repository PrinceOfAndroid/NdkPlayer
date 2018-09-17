//
// Created by PrinceOfAndroid on 2018/9/12 0012.
//

#ifndef NDKPLAYER_VIDEOCHANNEL_H
#define NDKPLAYER_VIDEOCHANNEL_H


#include "BaseChannel.h"
extern "C" {
#include <libswscale/swscale.h>
}

typedef void (*RenderFrameCallback)(uint8_t *, int, int, int);

class VideoChannel : public BaseChannel {
public:
    VideoChannel(int id, AVCodecContext *avCodecContext);

    ~VideoChannel(int id, AVCodecContext *avCodecContext);

    //解码+播放
    void play();

    void decode();

    void render();

    void setRenderFrameCallback(RenderFrameCallback callback);

private:
    pthread_t pid_decode;
    pthread_t pid_render;
    SafeQueue<AVFrame*> frames;
    SwsContext *swsContext;
    RenderFrameCallback callback;
};


#endif //NDKPLAYER_VIDEOCHANNEL_H
