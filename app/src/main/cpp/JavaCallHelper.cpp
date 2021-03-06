//
// Created by PrinceOfAndroid on 2018/9/11 0011.
//

#include "JavaCallHelper.h"
#include "macro.h"

JavaCallHelper::JavaCallHelper(JavaVM *vm, JNIEnv *env, jobject instance) {
    this->vm = vm;
    //如果在主线程回调
    this->env = env;
    //
    this->instance = env->NewGlobalRef(instance);

    jclass clazz = env->GetObjectClass(instance);
    onErrorId = env->GetMethodID(clazz, "onError", "(I)V");

    onPrepareId = env->GetMethodID(clazz, "onPrepare", "()V");
}

JavaCallHelper::~JavaCallHelper() {
    env->DeleteGlobalRef(instance);
}

void JavaCallHelper::onError(int thread, int errorCode) {
    if (thread == THREAD_MAIN) {
        env->CallVoidMethod(instance, onErrorId, errorCode);
    } else {
        JNIEnv *env;
        vm->AttachCurrentThread(&env, 0);
        env->CallVoidMethod(instance, onErrorId, errorCode);
        vm->DetachCurrentThread();
    }
}

void JavaCallHelper::onPrepare(int thread) {
    if (thread == THREAD_MAIN) {
        env->CallVoidMethod(instance, onPrepareId);
    } else {
        JNIEnv *env;
        vm->AttachCurrentThread(&env, 0);
        env->CallVoidMethod(instance, onPrepareId);
        vm->DetachCurrentThread();
    }
}