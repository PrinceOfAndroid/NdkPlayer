//
// Created by PrinceOfAndroid on 2018/9/11 0011.
//

#ifndef NDKPLAYER_JAVACALLHELPER_H
#define NDKPLAYER_JAVACALLHELPER_H


#include <jni.h>

class JavaCallHelper {
public:
    JavaCallHelper(JavaVM *vm, JNIEnv *env, jobject instance);

    ~JavaCallHelper();

    void onError(int thread, int errorCode);

    void onPrepare(int thread);

private:
    JavaVM *vm;
    JNIEnv *env;
    jobject instance;
    jmethodID onErrorId;
    jmethodID onPrepareId;
};


#endif //NDKPLAYER_JAVACALLHELPER_H
