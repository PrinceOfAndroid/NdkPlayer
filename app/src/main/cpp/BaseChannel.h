//
// Created by PrinceOfAndroid on 2018/9/13 0013.
//

#ifndef NDKPLAYER_BASECHANNEL_H
#define NDKPLAYER_BASECHANNEL_H

extern "C" {
#include <libavcodec/avcodec.h>
};

#include "safe_queue.h"

class BaseChannel {
public:
    BaseChannel(int id, AVCodecContext *avCodecContext) : id(id), avCodecContext(avCodecContext) {}

    virtual ~BaseChannel() {
        packets.setReleaseCallback(BaseChannel::releaseAvPacket);
        packets.clear();
    }

    static void releaseAvPacket(AVPacket **packet) {
        if (packet) {
            av_packet_free(packet);
            *packet = 0;
        }
    }

    static void releaseAvFrame(AVFrame **frame) {
        if (frame) {
            av_frame_free(frame);
            *frame = 0;
        }
    }

    virtual void play()=0;

    int id;
    SafeQueue<AVPacket *> packets;
    AVCodecContext *avCodecContext;
    bool isPlaying;
};

#endif //NDKPLAYER_BASECHANNEL_H
