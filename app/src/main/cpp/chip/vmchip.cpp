//
// Created by d.cn on 2020/12/23.
//
#include <ctime>
#include "vmchip.h"
#include "../jni_load.h"
#include "chip8.h"

#define TAG_VM "vmchip_native"
#define CAST(ptr) reinterpret_cast<Chip8 *>(ptr)

static void playSound() {
    JNIEnv *jniEnv = getEnv();
    if (!jniEnv)
        return;
    jclass clz = jniEnv->FindClass(CLASS_PATH);
    jmethodID mid = jniEnv->GetStaticMethodID(clz, "beep", "()V");
    jniEnv->CallStaticVoidMethod(clz, mid);
}

static jlong createChip8(JNIEnv *env, jclass clazz) {
    DLOG(TAG_VM, "chip8 created");
    Chip8 *chip8 = new Chip8;
    chip8->beep = playSound;
    return jlong(chip8);
}

static jboolean load(JNIEnv *env, jclass clazz, jlong ptr, jstring path) {
    const char *filePath = env->GetStringUTFChars(path, NULL);
    DFLOG(TAG_VM, "chip8 load %s", filePath);
    Chip8 *chip8 = CAST(ptr);
    chip8->rest();
    chip8->init();
    bool result = chip8->loadGame(filePath);
    return jboolean(result);
}

static jint loop(JNIEnv *env, jclass clazz, jlong ptr, jbyteArray pixs) {
    Chip8 *chip8 = CAST(ptr);
    chip8->loop();
    chip8->timeTick();
    if (chip8->drawFlag) {
        chip8->drawFlag = false;
        unsigned char *gfx = chip8->gfx;
        int size = sizeof(chip8->gfx);
        jbyte buf[size];
        for (int i = 0; i < size; i++) {
            buf[i] = jbyte(gfx[i]);
        }
        env->SetByteArrayRegion(pixs, 0, size, buf);
        return 1;
    }
    return 0;
}

static void destory(JNIEnv *env, jclass clazz, jlong ptr) {
    DLOG(TAG_VM, "chip8 destoryed");
    Chip8 *chip8 = CAST(ptr);
    delete chip8;
}

static void keyEvent(JNIEnv *, jclass, jlong ptr, jint index, jint pressed) {
    Chip8 *chip8 = CAST(ptr);
    chip8->keyEvent(index, pressed == 1);
}

JNINativeMethod getMethods[] = {
        {"jcreateChip8", "()J",                    (void *) createChip8},
        {"jload",        "(JLjava/lang/String;)Z", (void *) load},
        {"jloop",        "(J[B)I",                 (void *) loop},
        {"jdestory",     "(J)V",                   (void *) destory},
        {"jkeyEvent",    "(JII)V",                 (void *) keyEvent}
};

MenthodRegister registChipMethods() {
    MenthodRegister mr{CLASS_PATH,
                       getMethods,
                       sizeof(getMethods) / sizeof(JNINativeMethod)};
    return mr;
}