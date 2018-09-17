//
// Created by PrinceOfAndroid on 2018/9/11 0011.
//

#ifndef NDKPLAYER_DNFFMPEG_H
#define NDKPLAYER_DNFFMPEG_H

#include "JavaCallHelper.h"
#include "AudioChannel.h"
#include "VideoChannel.h"

extern "C" {
#include <libavformat/avformat.h>
}

class DNFFmpeg {
public:
    DNFFmpeg(JavaCallHelper *callHelper, const char *dataSource);

    ~DNFFmpeg();

    void prepare();

    void _prepare();

    void start();

    void _start();

    void setRenderFrameCallback(RenderFrameCallback callback)

private:
    char *dataSource;
    pthread_t pid;
    pthread_t pid_play;
    AVFormatContext *formatContext;
    JavaCallHelper *callHelper;
    AudioChannel *audioChannel = 0;
    VideoChannel *videoChannel = 0;
    bool isPlaying;
};


#endif //NDKPLAYER_DNFFMPEG_H
