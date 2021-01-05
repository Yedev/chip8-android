//
// Created by d.cn on 2020/12/24.
//

#ifndef CHIP8_JNI_LOAD_H
#define CHIP8_JNI_LOAD_H

#include <android/log.h>
#include <jni.h>

#define DLOG(tag, msg) __android_log_write(3,tag,msg)
#define DFLOG(tag, fmt, msg) __android_log_print(3,tag,fmt,msg)
struct MenthodRegister {
    const char *className;
    JNINativeMethod *methods;
    int size;
};

JNIEnv *getEnv();

#endif //CHIP8_JNI_LOAD_H
