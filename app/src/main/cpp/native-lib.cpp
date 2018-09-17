#include <jni.h>
#include <string>
#include <android/log.h>
#include <pthread.h>
#include <android/native_window_jni.h>
#include "DNFFmpeg.h"


DNFFmpeg *dnfFmpeg = 0;
ANativeWindow *window = 0;

extern "C" {
#include <libavcodec/avcodec.h>
}

extern "C" JNIEXPORT jstring


JNICALL
Java_com_taos_up_ndkplayer_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
//    std::string hello = "Hello from C++";

    return env->NewStringUTF(av_version_info());
}

JavaVM *_vm;

struct Context {
    jobject instance;

};

int JNI_OnLoad(JavaVM *vm, void *r) {
    __android_log_print(ANDROID_LOG_ERROR, "JNI", "JNI_Onload");
    _vm = vm;
    return JNI_VERSION_1_6;
}

void render(uint8_t *data, int linesize, int w, int h) {
    if (!window) {
        return;
    }
    ANativeWindow_setBuffersGeometry(window, w, h, WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer window_buffer;
    if (ANativeWindow_lock(window, &window_buffer, 0)) {
        ANativeWindow_release(window);
        window = 0;
        return;
    }

    uint8_t *dst_data = static_cast<uint8_t *>(window_buffer.bits);

    int dst_linesize = window_buffer.stride * 4;

    for (int i = 0; i < window_buffer.height; ++i) {
        memcpy(dst_data + i * dst_linesize, data + i * linesize, dst_linesize);
    }
}

void *threadTask(void *args) {
    //native线程 附加到java虚拟机
    JNIEnv *env;
    jint i = _vm->AttachCurrentThread(&env, 0);
    if (i != JNI_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "JNI", "附加失败");
        return 0;
    }

    //下载完成
    //下载失败
    Context *context = static_cast<Context *>(args);

    jclass cls = env->GetObjectClass(context->instance);
    __android_log_print(ANDROID_LOG_ERROR, "JNI", "启动线程1");
    jmethodID md = env->GetMethodID(cls, "updateUI", "()V");
    env->CallVoidMethod(context->instance, md);
    delete (context);

    context = 0;

    //删除全局引用
    env->DeleteGlobalRef(context->instance);
    //分离
    _vm->DetachCurrentThread();
    return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_taos_up_ndkplayer_MainActivity_testThread(JNIEnv *env, jobject instance) {
    pthread_t pid;
    //启动线程
    Context *context = new Context();
    context->instance = env->NewGlobalRef(instance);
    __android_log_print(ANDROID_LOG_ERROR, "JNI", "启动线程");
    pthread_create(&pid, 0, threadTask, context);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_taos_up_ndkplayer_DNPlayer_native_1prepare(JNIEnv *env, jobject instance,
                                                    jstring dataSource_) {
    const char *dataSource = env->GetStringUTFChars(dataSource_, 0);

    // TODO
    JavaCallHelper *callHelper = new JavaCallHelper(_vm, env, instance);
    dnfFmpeg = new DNFFmpeg(callHelper, dataSource);
    dnfFmpeg->prepare();
    dnfFmpeg->setRenderFrameCallback();
    env->ReleaseStringUTFChars(dataSource_, dataSource);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_taos_up_ndkplayer_DNPlayer_native_1start(JNIEnv *env, jobject instance) {

    dnfFmpeg->start();
    // TODO

}

extern "C"
JNIEXPORT void JNICALL
Java_com_taos_up_ndkplayer_DNPlayer_native_1setSurface(JNIEnv *env, jobject instance,
                                                       jobject surface) {
    if (window) {
        //将老的window释放
        ANativeWindow_release(window);
        window = 0;
    }
    window = ANativeWindow_fromSurface(env, surface);

}