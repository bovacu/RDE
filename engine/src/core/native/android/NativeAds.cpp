// Created by borja on 18/2/22.

#include "core/native/android/NativeAds.h"
#include "core/Core.h"

#if IS_ANDROID()
#include <jni.h>
#include <string>

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_android_MainActivity_stringFromJNI(JNIEnv *env, jobject thiz) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
#endif