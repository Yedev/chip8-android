//
// Created by d.cn on 2020/12/23.
//

#include <iostream>
#include <jni.h>
#include <string>
#include "jni_load.h"

extern MenthodRegister registChipMethods();

static int registerNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *getMethods,
                                 int methodsNum) {
    jclass clazz;
    clazz = env->FindClass(className);
    if (clazz == NULL) {
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, getMethods, methodsNum) < 0) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

static int regist(JNIEnv *env, MenthodRegister &mr) {
    return registerNativeMethods(env, mr.className, mr.methods, mr.size);
}

JavaVM *javaVm;

JNIEnv *getEnv() {
    JNIEnv *jniEnv = NULL;
    if (javaVm->GetEnv(reinterpret_cast<void **>(&jniEnv), JNI_VERSION_1_6) != JNI_OK) {
        return NULL;
    }
    return jniEnv;
}

JNIEXPORT jint
JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *jniEnv = NULL;
    javaVm = vm;
    if (vm->GetEnv(reinterpret_cast<void **>(&jniEnv), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    assert(jniEnv != NULL);
    //regist methods
    MenthodRegister chip = registChipMethods();
    if (!regist(jniEnv, chip)) {
        return -1;
    }
    return JNI_VERSION_1_6;
}

JNIEXPORT void
JNI_OnUnload(JavaVM *vm, void *reserved) {
    javaVm = NULL;
}
