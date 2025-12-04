#ifndef LXC_WRAPPER_H
#define LXC_WRAPPER_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved);
JNIEXPORT void JNI_OnUnload(JavaVM *vm, void *reserved);

// LXC version related
JNIEXPORT jstring JNICALL nativeGetVersion(JNIEnv *env, jclass clazz);

// Container management related
JNIEXPORT jobjectArray JNICALL nativeListContainers(JNIEnv *env, jclass clazz, jstring jlxcpath);
JNIEXPORT jboolean JNICALL nativeIsDefined(JNIEnv *env, jclass clazz, jstring jname, jstring jlxcpath);
JNIEXPORT jboolean JNICALL nativeIsRunning(JNIEnv *env, jclass clazz, jstring jname, jstring jlxcpath);
JNIEXPORT jstring JNICALL nativeGetState(JNIEnv *env, jclass clazz, jstring jname, jstring jlxcpath);
JNIEXPORT jboolean JNICALL nativeStartContainer(JNIEnv *env, jclass clazz, jstring jname, jstring jlxcpath, jboolean juseInit);
JNIEXPORT jboolean JNICALL nativeStopContainer(JNIEnv *env, jclass clazz, jstring jname, jstring jlxcpath);
JNIEXPORT jboolean JNICALL nativeFreezeContainer(JNIEnv *env, jclass clazz, jstring jname, jstring jlxcpath);
JNIEXPORT jboolean JNICALL nativeUnfreezeContainer(JNIEnv *env, jclass clazz, jstring jname, jstring jlxcpath);
JNIEXPORT jboolean JNICALL nativeDestroyContainer(JNIEnv *env, jclass clazz, jstring jname, jstring jlxcpath);
JNIEXPORT jstring JNICALL nativeGetConfigItem(JNIEnv *env, jclass clazz, jstring jname, jstring jlxcpath, jstring jkey);
JNIEXPORT jboolean JNICALL nativeSetConfigItem(JNIEnv *env, jclass clazz, jstring jname, jstring jlxcpath, jstring jkey, jstring jvalue);
JNIEXPORT jint JNICALL nativeCreateSnapshot(JNIEnv *env, jclass clazz, jstring jname, jstring jlxcpath);
JNIEXPORT jobjectArray JNICALL nativeGetInterfaces(JNIEnv *env, jclass clazz, jstring jname, jstring jlxcpath);

#ifdef __cplusplus
}
#endif

#endif // LXC_WRAPPER_H