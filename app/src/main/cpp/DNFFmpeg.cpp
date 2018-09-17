//
// Created by PrinceOfAndroid on 2018/9/11 0011.
//

#include <string.h>
#include <pthread.h>
#include "DNFFmpeg.h"
#include "macro.h"


void *task_prepare(void *args) {

    DNFFmpeg *dnfFmpeg = static_cast<DNFFmpeg *>(args);
    dnfFmpeg->_prepare();
    return 0;
}

DNFFmpeg::DNFFmpeg(JavaCallHelper *callHelper, const char *dataSource) {
    this->callHelper = callHelper;
    //防止dataSource指向的内存被释放
    //strlean 获得字符串的长度 不包括\0
    this->dataSource = new char[strlen(dataSource) + 1];
    strcpy(this->dataSource, dataSource);
}

DNFFmpeg::~DNFFmpeg() {
    //释放
    DELETE(dataSource)
    DELETE(callHelper)
}

void DNFFmpeg::prepare() {
    //创建一个线程
    pthread_create(&pid, 0, task_prepare, this);
}

void DNFFmpeg::_prepare() {
    //初始化网络
    avformat_network_init();
    //打开媒体地址（文件地址 直播地址）
    //formatContext 包含了视频的信息（宽高等）
    formatContext = 0;
    int ret = avformat_open_input(&formatContext, dataSource, 0, 0);
    //ret不为0表示 打开媒体失败
    if (ret != 0) {
        //回调安卓的onError方法
        LOGE("打开媒体失败", av_err2str(ret));
        callHelper->onError(THREAD_CHILD, FFMPEG_CAN_NOT_OPEN_URL);
        return;
    }
    //查找媒体中的音视频流
    ret = avformat_find_stream_info(formatContext, 0);
    if (ret != 0) {
        LOGE("获取音视频流失败", av_err2str(ret));
        callHelper->onError(THREAD_CHILD, FFMPEG_CAN_NOT_FIND_STREAMS);
        return;
    }

    //nb_streams几个流（几段视频或者音频）
    for (int i = 0; i < formatContext->nb_streams; ++i) {
        //可能代表是一个视频也可能是一个音频
        AVStream *avStream = formatContext->streams[i];
        //包含了解码这段流的各种参数信息
        AVCodecParameters *codecpar = avStream->codecpar;

        //无论视屏还是音频都需要干的一些事情（获得解码器）
        //1.通过当前流使用的编码方式，查找解码器
        AVCodec *dec = avcodec_find_decoder(codecpar->codec_id);
        if (dec == NULL) {
            LOGE("获取解码器失败", av_err2str(ret));
            callHelper->onError(THREAD_CHILD, FFMPEG_FIND_DECODER_FAIL);
            return;
        }
        //获得解码器上下文
        AVCodecContext *context = avcodec_alloc_context3(dec);
        if (context == NULL) {
            LOGE("获得解码器上下文失败", av_err2str(ret));
            callHelper->onError(THREAD_CHILD, FFMPEG_ALLOC_CODEC_CONTEXT_FAIL);
            return;
        }

        //设置上下文的一些参数
        ret = avcodec_parameters_to_context(context, codecpar);
        if (ret < 0) {
//            LOGE("打开媒体失败",av_err2str(ret));
            callHelper->onError(THREAD_CHILD, FFMPEG_CODEC_CONTEXT_PARAMETERS_FAIL);
            return;
        }

        //打开编码器  (第三个参数 特性)
        ret = avcodec_open2(context, dec, 0);
        if (ret != 0) {
            LOGE("打开编码器失败", av_err2str(ret));
            callHelper->onError(THREAD_CHILD, FFMPEG_OPEN_DECODER_FAIL);
        }


        if (codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioChannel = new AudioChannel(i,context);
        } else {
            videoChannel = new VideoChannel(i,context);
        }

        if (!audioChannel && !videoChannel) {
            callHelper->onError(THREAD_CHILD, FFMPEG_NOMEDIA);
            return;
        }


        //准备完了通知java
        callHelper->onPrepare(THREAD_CHILD);

    }

}

void *play(void *args) {
    DNFFmpeg *ffmpeg = static_cast<DNFFmpeg *>(args);
    ffmpeg->_start();
    return 0;
}

void DNFFmpeg::start() {
    //正在播放
    isPlaying = 1;
    if (videoChannel){
        videoChannel->packets.setWork(1);
    }
    pthread_create(&pid_play, 0, play, this);
}

void DNFFmpeg::_start() {
    int ret;
    //1.读取媒体数据包(音视频数据包)
    while (isPlaying) {
        AVPacket *packet = av_packet_alloc();
        ret = av_read_frame(formatContext, packet);
        //=0成功 其他失败
        if (ret == 0) {
            //stream_index流的一个序号
            if (audioChannel && packet->stream_index == audioChannel->id) {

            } else if (videoChannel && packet->stream_index == videoChannel->id) {
                videoChannel->packets.push(packet);
            }
        } else if (ret == AVERROR_EOF) {
            //读取完但是没有播放完
        } else {

        }
    }
}

void DNFFmpeg::setRenderFrameCallback(RenderFrameCallback callback) {
    if (videoChannel){
        videoChannel->setRenderFrameCallback(callback);
    }
}
