//
// Created by d.cn on 2020/12/23.
//
#ifndef CHIP8_VMCHIP_H
#define CHIP8_VMCHIP_H

#include <jni.h>

#define CLASS_PATH "com/ych/chip8/ChipJNI"

static jlong createChip8(JNIEnv *, jclass);

static jboolean load(JNIEnv *, jclass, jlong, jstring);

static jint loop(JNIEnv *, jclass, jlong, jbyteArray);

static void destory(JNIEnv *, jclass, jlong);

static void keyEvent(JNIEnv *, jclass, jlong, jint, jint);

static void playSound();

#endif //CHIP8_VMCHIP_H
