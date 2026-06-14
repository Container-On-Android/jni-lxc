#include <jni.h>
#include <stdint.h>
#include <lxc/lxccontainer.h>
#include <lxc/attach_options.h>
#include <lxc/version.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>

#include "lxc-wrapper.h"

#include "lxc-binding.h"

#ifndef LXC_DEVEL
#define LXC_DEVEL 0
#endif

static struct lxc_container* get_container(JNIEnv *env, jstring name, jstring lxcpath) {
    const char *container_name = (*env)->GetStringUTFChars(env, name, NULL);
    const char *container_lxcpath = (*env)->GetStringUTFChars(env, lxcpath, NULL);

    if (container_name == NULL || container_lxcpath == NULL) {
        if (container_name != NULL) {
            (*env)->ReleaseStringUTFChars(env, name, container_name);
        }
        if (container_lxcpath != NULL) {
            (*env)->ReleaseStringUTFChars(env, lxcpath, container_lxcpath);
        }
        return NULL;
    }

    struct lxc_container *c = lxc_container_new(container_name, container_lxcpath);

    (*env)->ReleaseStringUTFChars(env, name, container_name);
    (*env)->ReleaseStringUTFChars(env, lxcpath, container_lxcpath);

    return c;
}

JNIEXPORT jstring JNICALL
Java_io_github_coap_lxc_LxcNative_nativeGetVersion(JNIEnv *env, jclass clazz) {
    return (*env)->NewStringUTF(env, LXC_VERSION);
}

JNIEXPORT jobjectArray JNICALL
Java_io_github_coap_lxc_LxcNative_nativeListContainers(JNIEnv *env, jclass clazz, jstring lxcpath) {
    const char *container_lxcpath = (*env)->GetStringUTFChars(env, lxcpath, NULL);

    if (container_lxcpath == NULL)
        return NULL;

    char **names = NULL;
    struct lxc_container **containers = NULL;
    int count = list_all_containers(container_lxcpath, &names, &containers);
    (*env)->ReleaseStringUTFChars(env, lxcpath, container_lxcpath);

    if (count < 0) {
        return (*env)->NewObjectArray(env, 0, (*env)->FindClass(env, "java/lang/String"), NULL);
    }

    jclass string_class = (*env)->FindClass(env, "java/lang/String");
    jobjectArray result = (*env)->NewObjectArray(env, count, string_class, NULL);

    for (int i = 0; i < count; i++) {
        jstring jname = (*env)->NewStringUTF(env, names[i]);
        (*env)->SetObjectArrayElement(env, result, i, jname);
        free(names[i]);
    }
    free(names);
    if (containers != NULL) {
        for (int i = 0; i < count; i++) {
            if (containers[i] != NULL) {
                lxc_container_put(containers[i]);
            }
        }
        free(containers);
    }

    return result;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeIsDefined(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    bool result = go_lxc_defined(c);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeIsRunning(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    bool result = go_lxc_running(c);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jstring JNICALL
Java_io_github_coap_lxc_LxcNative_nativeGetState(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return (*env)->NewStringUTF(env, "UNKNOWN");
    }

    const char *state = go_lxc_state(c);
    jstring result = (*env)->NewStringUTF(env, state ? state : "UNKNOWN");
    lxc_container_put(c);
    return result;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeStartContainer(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jboolean useInit) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    if (!go_lxc_defined(c)) {
        lxc_container_put(c);
        return JNI_FALSE;
    }

    bool result = go_lxc_start(c, useInit ? 1 : 0, NULL);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeStopContainer(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    bool result = go_lxc_stop(c);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeFreezeContainer(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    bool result = go_lxc_freeze(c);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeUnfreezeContainer(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    bool result = go_lxc_unfreeze(c);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeDestroyContainer(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    bool result = go_lxc_destroy(c);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jstring JNICALL
Java_io_github_coap_lxc_LxcNative_nativeGetConfigItem(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jstring key) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return NULL;
    }

    const char *config_key = (*env)->GetStringUTFChars(env, key, NULL);
    if (config_key == NULL) {
        lxc_container_put(c);
        return NULL;
    }

    char *value = go_lxc_get_config_item(c, config_key);
    (*env)->ReleaseStringUTFChars(env, key, config_key);
    lxc_container_put(c);

    if (value == NULL) {
        return NULL;
    }

    jstring result = (*env)->NewStringUTF(env, value);
    free(value);
    return result;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeSetConfigItem(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jstring key, jstring value) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    const char *config_key = (*env)->GetStringUTFChars(env, key, NULL);
    const char *config_value = (*env)->GetStringUTFChars(env, value, NULL);

    if (config_key == NULL || config_value == NULL) {
        if (config_key != NULL) (*env)->ReleaseStringUTFChars(env, key, config_key);
        if (config_value != NULL) (*env)->ReleaseStringUTFChars(env, value, config_value);
        lxc_container_put(c);
        return JNI_FALSE;
    }

    bool result = go_lxc_set_config_item(c, config_key, config_value);

    (*env)->ReleaseStringUTFChars(env, key, config_key);
    (*env)->ReleaseStringUTFChars(env, value, config_value);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jint JNICALL
Java_io_github_coap_lxc_LxcNative_nativeCreateSnapshot(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return -1;
    }

    int result = go_lxc_snapshot(c);
    lxc_container_put(c);
    return result;
}

JNIEXPORT jobjectArray JNICALL
Java_io_github_coap_lxc_LxcNative_nativeGetInterfaces(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return (*env)->NewObjectArray(env, 0, (*env)->FindClass(env, "java/lang/String"), NULL);
    }

    char **interfaces = go_lxc_get_interfaces(c);
    if (interfaces == NULL) {
        lxc_container_put(c);
        return (*env)->NewObjectArray(env, 0, (*env)->FindClass(env, "java/lang/String"), NULL);
    }

    int count = 0;
    while (interfaces[count] != NULL) {
        count++;
    }

    jclass string_class = (*env)->FindClass(env, "java/lang/String");
    jobjectArray result = (*env)->NewObjectArray(env, count, string_class, NULL);

    for (int i = 0; i < count; i++) {
        jstring jinterface = (*env)->NewStringUTF(env, interfaces[i]);
        (*env)->SetObjectArrayElement(env, result, i, jinterface);
        free(interfaces[i]);
    }
    free(interfaces);

    lxc_container_put(c);
    return result;
}

JNIEXPORT jint JNICALL
Java_io_github_coap_lxc_LxcNative_nativeGetConsoleFd(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jint ttynum) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return -1;
    }

    int fd = go_lxc_console_getfd(c, ttynum);
    lxc_container_put(c);
    return fd;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeConsole(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jint ttynum, jint stdinfd, jint stdoutfd, jint stderrfd, jint escape) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    bool result = go_lxc_console(c, ttynum, stdinfd, stdoutfd, stderrfd, escape);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jint JNICALL
Java_io_github_coap_lxc_LxcNative_nativeAttachRunWait(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jboolean clearEnv, jint namespaces, jlong personality, jint uid, jint gid, jobjectArray argv, jint attachFlags) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return -1;
    }

    lxc_groups_t groups = {0, NULL};

    jsize argc = (*env)->GetArrayLength(env, argv);
    char **args = (char **)malloc(sizeof(char *) * (argc + 1));
    if (args == NULL) {
        lxc_container_put(c);
        return -1;
    }

    for (int i = 0; i < argc; i++) {
        jstring jarg = (jstring)(*env)->GetObjectArrayElement(env, argv, i);
        args[i] = (char *)(*env)->GetStringUTFChars(env, jarg, NULL);
    }
    args[argc] = NULL;

    int result = go_lxc_attach_run_wait(c, clearEnv ? true : false, namespaces, personality,
                                        uid, gid, groups, 0, 0, 0, NULL, NULL, NULL, (const char *const *)args, attachFlags);

    for (int i = 0; i < argc; i++) {
        jstring jarg = (jstring)(*env)->GetObjectArrayElement(env, argv, i);
        (*env)->ReleaseStringUTFChars(env, jarg, args[i]);
    }
    free(args);
    lxc_container_put(c);

    return result;
}

JNIEXPORT jint JNICALL
Java_io_github_coap_lxc_LxcNative_nativeAttachNoWait(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jboolean clearEnv, jint namespaces, jlong personality, jint uid, jint gid, jobjectArray argv, jint attachFlags) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return -1;
    }

    lxc_groups_t groups = {0, NULL};
    pid_t attached_pid;

    jsize argc = (*env)->GetArrayLength(env, argv);
    char **args = (char **)malloc(sizeof(char *) * (argc + 1));
    if (args == NULL) {
        lxc_container_put(c);
        return -1;
    }

    for (int i = 0; i < argc; i++) {
        jstring jarg = (jstring)(*env)->GetObjectArrayElement(env, argv, i);
        args[i] = (char *)(*env)->GetStringUTFChars(env, jarg, NULL);
    }
    args[argc] = NULL;

    int ret = go_lxc_attach_no_wait(c, clearEnv ? true : false, namespaces, personality,
                                     uid, gid, groups, 0, 0, 0, NULL, NULL, NULL, (const char *const *)args, &attached_pid, attachFlags);

    for (int i = 0; i < argc; i++) {
        jstring jarg = (jstring)(*env)->GetObjectArrayElement(env, argv, i);
        (*env)->ReleaseStringUTFChars(env, jarg, args[i]);
    }
    free(args);
    lxc_container_put(c);

    if (ret < 0) {
        return -1;
    }
    return (jint)attached_pid;
}

JNIEXPORT jint JNICALL
Java_io_github_coap_lxc_LxcNative_nativeAttach(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jboolean clearEnv, jint namespaces, jlong personality, jint uid, jint gid, jint stdinfd, jint stdoutfd, jint stderrfd, jint attachFlags) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return -1;
    }

    lxc_groups_t groups = {0, NULL};

    int result = go_lxc_attach(c, clearEnv ? true : false, namespaces, personality,
                               uid, gid, groups, stdinfd, stdoutfd, stderrfd,
                               NULL, NULL, NULL, attachFlags);

    lxc_container_put(c);
    return result;
}

JNIEXPORT jint JNICALL
Java_io_github_coap_lxc_LxcNative_nativeGetErrorNum(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return -1;
    }

    int result = go_lxc_error_num(c);
    lxc_container_put(c);
    return result;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeAddDeviceNode(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jstring srcPath, jstring destPath) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    const char *src = srcPath == NULL ? NULL : (*env)->GetStringUTFChars(env, srcPath, NULL);
    const char *dst = destPath == NULL ? NULL : (*env)->GetStringUTFChars(env, destPath, NULL);

    bool result = go_lxc_add_device_node(c, src, dst);

    if (src != NULL) (*env)->ReleaseStringUTFChars(env, srcPath, src);
    if (dst != NULL) (*env)->ReleaseStringUTFChars(env, destPath, dst);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeRemoveDeviceNode(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jstring srcPath, jstring destPath) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    const char *src = srcPath == NULL ? NULL : (*env)->GetStringUTFChars(env, srcPath, NULL);
    const char *dst = destPath == NULL ? NULL : (*env)->GetStringUTFChars(env, destPath, NULL);

    bool result = go_lxc_remove_device_node(c, src, dst);

    if (src != NULL) (*env)->ReleaseStringUTFChars(env, srcPath, src);
    if (dst != NULL) (*env)->ReleaseStringUTFChars(env, destPath, dst);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeClone(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jstring newname, jstring newlxcpath, jint flags, jstring bdevtype) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    const char *newname_str = (*env)->GetStringUTFChars(env, newname, NULL);
    const char *newlxcpath_str = newlxcpath == NULL ? NULL : (*env)->GetStringUTFChars(env, newlxcpath, NULL);
    const char *bdevtype_str = bdevtype == NULL ? NULL : (*env)->GetStringUTFChars(env, bdevtype, NULL);

    bool result = go_lxc_clone(c, newname_str, newlxcpath_str, flags, bdevtype_str);

    (*env)->ReleaseStringUTFChars(env, newname, newname_str);
    if (newlxcpath != NULL) (*env)->ReleaseStringUTFChars(env, newlxcpath, newlxcpath_str);
    if (bdevtype != NULL) (*env)->ReleaseStringUTFChars(env, bdevtype, bdevtype_str);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeReboot(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    bool result = go_lxc_reboot(c);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeShutdown(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jint timeout) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    bool result = go_lxc_shutdown(c, timeout);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeWait(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jstring state, jint timeout) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    const char *state_str = (*env)->GetStringUTFChars(env, state, NULL);
    bool result = go_lxc_wait(c, state_str, timeout);
    (*env)->ReleaseStringUTFChars(env, state, state_str);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeSaveConfig(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jstring altFile) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    const char *alt_file = altFile == NULL ? NULL : (*env)->GetStringUTFChars(env, altFile, NULL);
    bool result = go_lxc_save_config(c, alt_file);
    if (altFile != NULL) (*env)->ReleaseStringUTFChars(env, altFile, alt_file);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeLoadConfig(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jstring altFile) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    const char *alt_file = altFile == NULL ? NULL : (*env)->GetStringUTFChars(env, altFile, NULL);
    bool result = go_lxc_load_config(c, alt_file);
    if (altFile != NULL) (*env)->ReleaseStringUTFChars(env, altFile, alt_file);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeRename(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jstring newname) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    const char *newname_str = (*env)->GetStringUTFChars(env, newname, NULL);
    bool result = go_lxc_rename(c, newname_str);
    (*env)->ReleaseStringUTFChars(env, newname, newname_str);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeCheckpoint(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jstring directory, jboolean stop, jboolean verbose) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    const char *dir = (*env)->GetStringUTFChars(env, directory, NULL);
    bool result = go_lxc_checkpoint(c, (char *)dir, stop ? true : false, verbose ? true : false);
    (*env)->ReleaseStringUTFChars(env, directory, dir);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeRestore(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jstring directory, jboolean verbose) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    const char *dir = (*env)->GetStringUTFChars(env, directory, NULL);
    bool result = go_lxc_restore(c, (char *)dir, verbose ? true : false);
    (*env)->ReleaseStringUTFChars(env, directory, dir);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jobjectArray JNICALL
Java_io_github_coap_lxc_LxcNative_nativeGetIps(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jstring interface_, jstring family, jint scope) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return (*env)->NewObjectArray(env, 0, (*env)->FindClass(env, "java/lang/String"), NULL);
    }

    const char *iface = interface_ == NULL ? NULL : (*env)->GetStringUTFChars(env, interface_, NULL);
    const char *fam = family == NULL ? NULL : (*env)->GetStringUTFChars(env, family, NULL);

    char **ips = go_lxc_get_ips(c, iface, fam, scope);

    if (iface != NULL) (*env)->ReleaseStringUTFChars(env, interface_, iface);
    if (fam != NULL) (*env)->ReleaseStringUTFChars(env, family, fam);

    if (ips == NULL) {
        lxc_container_put(c);
        return (*env)->NewObjectArray(env, 0, (*env)->FindClass(env, "java/lang/String"), NULL);
    }

    int count = 0;
    while (ips[count] != NULL) {
        count++;
    }

    jclass string_class = (*env)->FindClass(env, "java/lang/String");
    jobjectArray result = (*env)->NewObjectArray(env, count, string_class, NULL);

    for (int i = 0; i < count; i++) {
        jstring jip = (*env)->NewStringUTF(env, ips[i]);
        (*env)->SetObjectArrayElement(env, result, i, jip);
        free(ips[i]);
    }
    free(ips);

    lxc_container_put(c);
    return result;
}

JNIEXPORT jstring JNICALL
Java_io_github_coap_lxc_LxcNative_nativeGetKeys(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jstring key) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return NULL;
    }

    const char *key_str = (*env)->GetStringUTFChars(env, key, NULL);
    char *value = go_lxc_get_keys(c, key_str);
    (*env)->ReleaseStringUTFChars(env, key, key_str);
    lxc_container_put(c);

    if (value == NULL) {
        return NULL;
    }

    jstring result = (*env)->NewStringUTF(env, value);
    free(value);
    return result;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeClearConfig(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    go_lxc_clear_config(c);
    lxc_container_put(c);
    return JNI_TRUE;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeClearConfigItem(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jstring key) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    const char *key_str = (*env)->GetStringUTFChars(env, key, NULL);
    bool result = go_lxc_clear_config_item(c, key_str);
    (*env)->ReleaseStringUTFChars(env, key, key_str);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeSetCgroupItem(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jstring key, jstring value) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    const char *key_str = (*env)->GetStringUTFChars(env, key, NULL);
    const char *value_str = (*env)->GetStringUTFChars(env, value, NULL);

    bool result = go_lxc_set_cgroup_item(c, key_str, value_str);

    (*env)->ReleaseStringUTFChars(env, key, key_str);
    (*env)->ReleaseStringUTFChars(env, value, value_str);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jstring JNICALL
Java_io_github_coap_lxc_LxcNative_nativeGetCgroupItem(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jstring key) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return NULL;
    }

    const char *key_str = (*env)->GetStringUTFChars(env, key, NULL);
    char *value = go_lxc_get_cgroup_item(c, key_str);
    (*env)->ReleaseStringUTFChars(env, key, key_str);
    lxc_container_put(c);

    if (value == NULL) {
        return NULL;
    }

    jstring result = (*env)->NewStringUTF(env, value);
    free(value);
    return result;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeMayControl(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    bool result = go_lxc_may_control(c);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jint JNICALL
Java_io_github_coap_lxc_LxcNative_nativeInitPid(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return -1;
    }

    pid_t pid = go_lxc_init_pid(c);
    lxc_container_put(c);
    return (jint)pid;
}

JNIEXPORT jint JNICALL
Java_io_github_coap_lxc_LxcNative_nativeSnapshotList(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return -1;
    }

    struct lxc_snapshot *snapshots = NULL;
    int count = go_lxc_snapshot_list(c, &snapshots);
    lxc_container_put(c);

    return count;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeSnapshotDestroy(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jstring snapname) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    const char *snapname_str = (*env)->GetStringUTFChars(env, snapname, NULL);
    bool result = go_lxc_snapshot_destroy(c, snapname_str);
    (*env)->ReleaseStringUTFChars(env, snapname, snapname_str);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeSnapshotDestroyAll(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    bool result = go_lxc_snapshot_destroy_all(c);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeSnapshotRestore(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jstring snapname, jstring newname) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    const char *snapname_str = (*env)->GetStringUTFChars(env, snapname, NULL);
    const char *newname_str = (*env)->GetStringUTFChars(env, newname, NULL);

    bool result = go_lxc_snapshot_restore(c, snapname_str, newname_str);

    (*env)->ReleaseStringUTFChars(env, snapname, snapname_str);
    (*env)->ReleaseStringUTFChars(env, newname, newname_str);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeAttachInterface(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jstring dev, jstring dstDev) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    const char *dev_str = (*env)->GetStringUTFChars(env, dev, NULL);
    const char *dst_dev_str = (*env)->GetStringUTFChars(env, dstDev, NULL);

    bool result = go_lxc_attach_interface(c, dev_str, dst_dev_str);

    (*env)->ReleaseStringUTFChars(env, dev, dev_str);
    (*env)->ReleaseStringUTFChars(env, dstDev, dst_dev_str);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeDetachInterface(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jstring dev, jstring dstDev) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    const char *dev_str = (*env)->GetStringUTFChars(env, dev, NULL);
    const char *dst_dev_str = (*env)->GetStringUTFChars(env, dstDev, NULL);

    bool result = go_lxc_detach_interface(c, dev_str, dst_dev_str);

    (*env)->ReleaseStringUTFChars(env, dev, dev_str);
    (*env)->ReleaseStringUTFChars(env, dstDev, dst_dev_str);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeWantDaemonize(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jboolean state) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    bool result = go_lxc_want_daemonize(c, state ? true : false);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeWantCloseAllFds(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jboolean state) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    bool result = go_lxc_want_close_all_fds(c, state ? true : false);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jstring JNICALL
Java_io_github_coap_lxc_LxcNative_nativeConfigFileName(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return NULL;
    }

    char *filename = go_lxc_config_file_name(c);
    lxc_container_put(c);

    if (filename == NULL) {
        return NULL;
    }

    jstring result = (*env)->NewStringUTF(env, filename);
    free(filename);
    return result;
}

JNIEXPORT jstring JNICALL
Java_io_github_coap_lxc_LxcNative_nativeGetConfigPath(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return NULL;
    }

    const char *path = go_lxc_get_config_path(c);
    jstring result = (*env)->NewStringUTF(env, path);
    lxc_container_put(c);
    return result;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeSetConfigPath(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jstring path) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    const char *path_str = (*env)->GetStringUTFChars(env, path, NULL);
    bool result = go_lxc_set_config_path(c, path_str);
    (*env)->ReleaseStringUTFChars(env, path, path_str);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jint JNICALL
Java_io_github_coap_lxc_LxcNative_nativeInitPidfd(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return -1;
    }

    int fd = go_lxc_init_pidfd(c);
    lxc_container_put(c);
    return fd;
}

JNIEXPORT jint JNICALL
Java_io_github_coap_lxc_LxcNative_nativeDevptsFd(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return -1;
    }

    int fd = go_lxc_devpts_fd(c);
    lxc_container_put(c);
    return fd;
}

JNIEXPORT jint JNICALL
Java_io_github_coap_lxc_LxcNative_nativeSeccompNotifyFd(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return -1;
    }

    int fd = go_lxc_seccomp_notify_fd(c);
    lxc_container_put(c);
    return fd;
}

JNIEXPORT jint JNICALL
Java_io_github_coap_lxc_LxcNative_nativeSeccompNotifyFdActive(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return -1;
    }

    int fd = go_lxc_seccomp_notify_fd_active(c);
    lxc_container_put(c);
    return fd;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeSetTimeout(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jint timeout) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return -1;
    }

    int result = go_lxc_set_timeout(c, timeout);
    lxc_container_put(c);
    return result;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeConfigItemIsSupported(JNIEnv *env, jclass clazz, jstring key) {
    const char *key_str = (*env)->GetStringUTFChars(env, key, NULL);
    bool result = go_lxc_config_item_is_supported(key_str);
    (*env)->ReleaseStringUTFChars(env, key, key_str);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeHasApiExtension(JNIEnv *env, jclass clazz, jstring extension) {
    const char *ext = (*env)->GetStringUTFChars(env, extension, NULL);
    bool result = go_lxc_has_api_extension(ext);
    (*env)->ReleaseStringUTFChars(env, extension, ext);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeCreate(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath, jstring t, jstring bdevtype, jint flags, jobjectArray argv) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    const char *t_str = t == NULL ? NULL : (*env)->GetStringUTFChars(env, t, NULL);
    const char *bdevtype_str = bdevtype == NULL ? NULL : (*env)->GetStringUTFChars(env, bdevtype, NULL);

    jsize argc = argv == NULL ? 0 : (*env)->GetArrayLength(env, argv);
    char **args = NULL;
    if (argc > 0) {
        args = (char **)malloc(sizeof(char *) * (argc + 1));
        if (args == NULL) {
            if (t_str != NULL) (*env)->ReleaseStringUTFChars(env, t, t_str);
            if (bdevtype_str != NULL) (*env)->ReleaseStringUTFChars(env, bdevtype, bdevtype_str);
            lxc_container_put(c);
            return JNI_FALSE;
        }
        for (int i = 0; i < argc; i++) {
            jstring jarg = (jstring)(*env)->GetObjectArrayElement(env, argv, i);
            args[i] = (char *)(*env)->GetStringUTFChars(env, jarg, NULL);
        }
        args[argc] = NULL;
    }

    bool result = go_lxc_create(c, t_str, bdevtype_str, NULL, flags, args);

    if (args != NULL) {
        for (int i = 0; i < argc; i++) {
            jstring jarg = (jstring)(*env)->GetObjectArrayElement(env, argv, i);
            (*env)->ReleaseStringUTFChars(env, jarg, args[i]);
        }
        free(args);
    }

    if (t_str != NULL) (*env)->ReleaseStringUTFChars(env, t, t_str);
    if (bdevtype_str != NULL) (*env)->ReleaseStringUTFChars(env, bdevtype, bdevtype_str);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_io_github_coap_lxc_LxcNative_nativeDestroyWithSnapshots(JNIEnv *env, jclass clazz, jstring name, jstring lxcpath) {
    struct lxc_container *c = get_container(env, name, lxcpath);
    if (c == NULL) {
        return JNI_FALSE;
    }

    bool result = go_lxc_destroy_with_snapshots(c);
    lxc_container_put(c);
    return result ? JNI_TRUE : JNI_FALSE;
}

/* ----------------------------------------------------------------------------
 * LXC monitor (talks to lxc-monitord over an abstract Unix domain socket).
 *
 * Wire format (struct lxc_msg):
 *   int           type   — enum lxc_msg_type (0 = lxc_msg_state, 1 = lxc_msg_exit_code)
 *   int           value  — lxc_state_t for lxc_msg_state, exit code otherwise
 *   char name[4096]      — NUL-terminated container name
 *
 * lxc-monitord reads sizeof(struct lxc_msg) from the publisher FIFO and
 * writes the same to every subscribed client fd, so a single lxc_monitor_read
 * yields the full record. This is what the lxc-monitor CLI tool relies on.
 *
 * liblxc.a exports strong implementations of lxc_monitor_open/close/read and
 * lxc_state2str. The weak fallbacks below are kept for the case where the
 * project is built without those symbols.
 * --------------------------------------------------------------------------*/

struct lxc_monitor {
    int fd;
};

static int lxc_abstract_unix_connect(const char *path) {
    int fd;
    struct sockaddr_un addr;
    size_t path_len;

    fd = socket(PF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        return -1;
    }

    path_len = strlen(path);
    if (path_len >= sizeof(addr.sun_path)) {
        close(fd);
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    /* Abstract namespace: leading NUL byte, then the literal path. */
    addr.sun_path[0] = '\0';
    memcpy(addr.sun_path + 1, path, path_len);

    if (connect(fd, (struct sockaddr *)&addr,
                offsetof(struct sockaddr_un, sun_path) + 1 + path_len) < 0) {
        close(fd);
        return -1;
    }
    return fd;
}

/* Read exactly n bytes from fd, retrying on partial reads. */
static ssize_t lxc_read_exact(int fd, void *buf, size_t n) {
    size_t got = 0;
    while (got < n) {
        ssize_t r = read(fd, (char *)buf + got, n - got);
        if (r < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (r == 0) {
            return -1; /* unexpected EOF */
        }
        got += (size_t)r;
    }
    return (ssize_t)got;
}

__attribute__((weak)) struct lxc_monitor *lxc_monitor_open(const char *lxcpath) {
    if (lxcpath == NULL) {
        return NULL;
    }

    char sock_path[PATH_MAX];
    int written = snprintf(sock_path, sizeof(sock_path),
                           "%s/monitor-sock", lxcpath);
    if (written <= 0 || written >= (int)sizeof(sock_path)) {
        return NULL;
    }

    int fd = lxc_abstract_unix_connect(sock_path);
    if (fd < 0) {
        return NULL;
    }

    struct lxc_monitor *mon = calloc(1, sizeof(*mon));
    if (mon == NULL) {
        close(fd);
        return NULL;
    }
    mon->fd = fd;
    return mon;
}

__attribute__((weak)) int lxc_monitor_close(struct lxc_monitor *mon) {
    if (mon == NULL) {
        return -1;
    }
    if (mon->fd >= 0) {
        /* Tell lxc-monitord we're done. Best effort: ignore errors. */
        ssize_t r = write(mon->fd, "quit", 4);
        (void)r;
        close(mon->fd);
        mon->fd = -1;
    }
    free(mon);
    return 0;
}

__attribute__((weak)) int lxc_monitor_read(struct lxc_monitor *mon, struct lxc_msg *msg) {
    if (mon == NULL || msg == NULL || mon->fd < 0) {
        return -1;
    }
    /* monitord writes the full struct in one shot. */
    if (lxc_read_exact(mon->fd, msg, sizeof(*msg)) < 0) {
        return -1;
    }
    msg->name[sizeof(msg->name) - 1] = '\0';
    return 0;
}

__attribute__((weak)) const char *lxc_state2str(int state) {
    switch (state) {
        case 0: return "STOPPED";
        case 1: return "STARTING";
        case 2: return "RUNNING";
        case 3: return "STOPPING";
        case 4: return "ABORTING";
        case 5: return "FREEZING";
        case 6: return "FROZEN";
        case 7: return "THAWED";
        case 8: return "MAX_STATE";
        default: return "UNKNOWN";
    }
}

JNIEXPORT jlong JNICALL
Java_io_github_coap_lxc_LxcNative_nativeOpenMonitor(JNIEnv *env, jclass clazz, jstring lxcpath) {
    const char *lxcpath_str = lxcpath == NULL ? NULL : (*env)->GetStringUTFChars(env, lxcpath, NULL);
    if (lxcpath_str == NULL) {
        return 0;
    }

    struct lxc_monitor *mon = lxc_monitor_open(lxcpath_str);
    (*env)->ReleaseStringUTFChars(env, lxcpath, lxcpath_str);
    return (jlong)(intptr_t)mon;
}

JNIEXPORT jint JNICALL
Java_io_github_coap_lxc_LxcNative_nativeCloseMonitor(JNIEnv *env, jclass clazz, jlong handle) {
    struct lxc_monitor *mon = (struct lxc_monitor *)(intptr_t)handle;
    if (mon == NULL) {
        return -1;
    }
    return lxc_monitor_close(mon);
}

JNIEXPORT jobjectArray JNICALL
Java_io_github_coap_lxc_LxcNative_nativeReadMonitorEvent(JNIEnv *env, jclass clazz, jlong handle) {
    struct lxc_monitor *mon = (struct lxc_monitor *)(intptr_t)handle;
    if (mon == NULL) {
        return NULL;
    }

    /* sizeof(struct lxc_msg) already includes the full name[] array. */
    struct lxc_msg *msg = malloc(sizeof(*msg));
    if (msg == NULL) {
        return NULL;
    }
    memset(msg, 0, sizeof(*msg));

    int ret = lxc_monitor_read(mon, msg);
    if (ret < 0) {
        free(msg);
        return NULL;
    }

    char type_str[16];
    snprintf(type_str, sizeof(type_str), "%d", (int)msg->type);

    char value_str[16];
    snprintf(value_str, sizeof(value_str), "%d", msg->value);

    const char *state_str = (msg->type == lxc_msg_state)
        ? lxc_state2str(msg->value)
        : "";

    jclass string_class = (*env)->FindClass(env, "java/lang/String");
    jobjectArray result = (*env)->NewObjectArray(env, 4, string_class, NULL);
    (*env)->SetObjectArrayElement(env, result, 0, (*env)->NewStringUTF(env, type_str));
    (*env)->SetObjectArrayElement(env, result, 1, (*env)->NewStringUTF(env, msg->name));
    (*env)->SetObjectArrayElement(env, result, 2, (*env)->NewStringUTF(env, state_str));
    (*env)->SetObjectArrayElement(env, result, 3, (*env)->NewStringUTF(env, value_str));

    free(msg);
    return result;
}

static JNINativeMethod gMethods[] = {
    {"nativeGetVersion", "()Ljava/lang/String;", (void *)Java_io_github_coap_lxc_LxcNative_nativeGetVersion},
    {"nativeListContainers", "(Ljava/lang/String;)[Ljava/lang/String;", (void *)Java_io_github_coap_lxc_LxcNative_nativeListContainers},
    {"nativeIsDefined", "(Ljava/lang/String;Ljava/lang/String;)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeIsDefined},
    {"nativeIsRunning", "(Ljava/lang/String;Ljava/lang/String;)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeIsRunning},
    {"nativeGetState", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;", (void *)Java_io_github_coap_lxc_LxcNative_nativeGetState},
    {"nativeStartContainer", "(Ljava/lang/String;Ljava/lang/String;Z)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeStartContainer},
    {"nativeStopContainer", "(Ljava/lang/String;Ljava/lang/String;)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeStopContainer},
    {"nativeFreezeContainer", "(Ljava/lang/String;Ljava/lang/String;)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeFreezeContainer},
    {"nativeUnfreezeContainer", "(Ljava/lang/String;Ljava/lang/String;)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeUnfreezeContainer},
    {"nativeDestroyContainer", "(Ljava/lang/String;Ljava/lang/String;)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeDestroyContainer},
    {"nativeGetConfigItem", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;", (void *)Java_io_github_coap_lxc_LxcNative_nativeGetConfigItem},
    {"nativeSetConfigItem", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeSetConfigItem},
    {"nativeCreateSnapshot", "(Ljava/lang/String;Ljava/lang/String;)I", (void *)Java_io_github_coap_lxc_LxcNative_nativeCreateSnapshot},
    {"nativeGetInterfaces", "(Ljava/lang/String;Ljava/lang/String;)[Ljava/lang/String;", (void *)Java_io_github_coap_lxc_LxcNative_nativeGetInterfaces},
    {"nativeGetConsoleFd", "(Ljava/lang/String;Ljava/lang/String;I)I", (void *)Java_io_github_coap_lxc_LxcNative_nativeGetConsoleFd},
    {"nativeConsole", "(Ljava/lang/String;Ljava/lang/String;IIIII)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeConsole},
    {"nativeAttachRunWait", "(Ljava/lang/String;Ljava/lang/String;ZIJII[Ljava/lang/String;I)I", (void *)Java_io_github_coap_lxc_LxcNative_nativeAttachRunWait},
    {"nativeAttachNoWait", "(Ljava/lang/String;Ljava/lang/String;ZIJII[Ljava/lang/String;I)I", (void *)Java_io_github_coap_lxc_LxcNative_nativeAttachNoWait},
    {"nativeAttach", "(Ljava/lang/String;Ljava/lang/String;ZIJIIIIII)I", (void *)Java_io_github_coap_lxc_LxcNative_nativeAttach},
    {"nativeGetErrorNum", "(Ljava/lang/String;Ljava/lang/String;)I", (void *)Java_io_github_coap_lxc_LxcNative_nativeGetErrorNum},
    {"nativeAddDeviceNode", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeAddDeviceNode},
    {"nativeRemoveDeviceNode", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeRemoveDeviceNode},
    {"nativeClone", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeClone},
    {"nativeReboot", "(Ljava/lang/String;Ljava/lang/String;)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeReboot},
    {"nativeShutdown", "(Ljava/lang/String;Ljava/lang/String;I)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeShutdown},
    {"nativeWait", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeWait},
    {"nativeSaveConfig", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeSaveConfig},
    {"nativeLoadConfig", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeLoadConfig},
    {"nativeRename", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeRename},
    {"nativeCheckpoint", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;ZZ)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeCheckpoint},
    {"nativeRestore", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Z)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeRestore},
    {"nativeGetIps", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)[Ljava/lang/String;", (void *)Java_io_github_coap_lxc_LxcNative_nativeGetIps},
    {"nativeGetKeys", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;", (void *)Java_io_github_coap_lxc_LxcNative_nativeGetKeys},
    {"nativeClearConfig", "(Ljava/lang/String;Ljava/lang/String;)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeClearConfig},
    {"nativeClearConfigItem", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeClearConfigItem},
    {"nativeSetCgroupItem", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeSetCgroupItem},
    {"nativeGetCgroupItem", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;", (void *)Java_io_github_coap_lxc_LxcNative_nativeGetCgroupItem},
    {"nativeMayControl", "(Ljava/lang/String;Ljava/lang/String;)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeMayControl},
    {"nativeInitPid", "(Ljava/lang/String;Ljava/lang/String;)I", (void *)Java_io_github_coap_lxc_LxcNative_nativeInitPid},
    {"nativeSnapshotList", "(Ljava/lang/String;Ljava/lang/String;)I", (void *)Java_io_github_coap_lxc_LxcNative_nativeSnapshotList},
    {"nativeSnapshotDestroy", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeSnapshotDestroy},
    {"nativeSnapshotDestroyAll", "(Ljava/lang/String;Ljava/lang/String;)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeSnapshotDestroyAll},
    {"nativeSnapshotRestore", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeSnapshotRestore},
    {"nativeAttachInterface", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeAttachInterface},
    {"nativeDetachInterface", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeDetachInterface},
    {"nativeWantDaemonize", "(Ljava/lang/String;Ljava/lang/String;Z)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeWantDaemonize},
    {"nativeWantCloseAllFds", "(Ljava/lang/String;Ljava/lang/String;Z)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeWantCloseAllFds},
    {"nativeConfigFileName", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;", (void *)Java_io_github_coap_lxc_LxcNative_nativeConfigFileName},
    {"nativeGetConfigPath", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;", (void *)Java_io_github_coap_lxc_LxcNative_nativeGetConfigPath},
    {"nativeSetConfigPath", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeSetConfigPath},
    {"nativeInitPidfd", "(Ljava/lang/String;Ljava/lang/String;)I", (void *)Java_io_github_coap_lxc_LxcNative_nativeInitPidfd},
    {"nativeDevptsFd", "(Ljava/lang/String;Ljava/lang/String;)I", (void *)Java_io_github_coap_lxc_LxcNative_nativeDevptsFd},
    {"nativeSeccompNotifyFd", "(Ljava/lang/String;Ljava/lang/String;)I", (void *)Java_io_github_coap_lxc_LxcNative_nativeSeccompNotifyFd},
    {"nativeSeccompNotifyFdActive", "(Ljava/lang/String;Ljava/lang/String;)I", (void *)Java_io_github_coap_lxc_LxcNative_nativeSeccompNotifyFdActive},
    {"nativeSetTimeout", "(Ljava/lang/String;Ljava/lang/String;I)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeSetTimeout},
    {"nativeConfigItemIsSupported", "(Ljava/lang/String;)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeConfigItemIsSupported},
    {"nativeHasApiExtension", "(Ljava/lang/String;)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeHasApiExtension},
    {"nativeCreate", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I[Ljava/lang/String;)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeCreate},
    {"nativeDestroyWithSnapshots", "(Ljava/lang/String;Ljava/lang/String;)Z", (void *)Java_io_github_coap_lxc_LxcNative_nativeDestroyWithSnapshots},
    {"nativeOpenMonitor", "(Ljava/lang/String;)J", (void *)Java_io_github_coap_lxc_LxcNative_nativeOpenMonitor},
    {"nativeCloseMonitor", "(J)I", (void *)Java_io_github_coap_lxc_LxcNative_nativeCloseMonitor},
    {"nativeReadMonitorEvent", "(J)[Ljava/lang/String;", (void *)Java_io_github_coap_lxc_LxcNative_nativeReadMonitorEvent},
};

static const char *kClassName = "io/github/coap/lxc/LxcNative";

JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;

    if ((*vm)->GetEnv(vm, (void **)&env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

    jclass clazz = (*env)->FindClass(env, kClassName);
    if (clazz == NULL) {
        return JNI_ERR;
    }

    if ((*env)->RegisterNatives(env, clazz, gMethods, sizeof(gMethods) / sizeof(gMethods[0])) != JNI_OK) {
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}
