//
// Created by PrinceOfAndroid on 2018/9/12 0012.
//

#ifndef NDKPLAYER_AUDIOCHANNEL_H
#define NDKPLAYER_AUDIOCHANNEL_H


#include "BaseChannel.h"

class AudioChannel : public BaseChannel{
public:
    AudioChannel(int id,AVCodecContext *avCodecContext);
    void play();
};


#endif //NDKPLAYER_AUDIOCHANNEL_H
