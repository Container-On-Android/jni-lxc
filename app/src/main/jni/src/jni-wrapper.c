#include <jni.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include <lxc/lxccontainer.h>
#include <lxc/attach_options.h>
#include <lxc/version.h>

#include "lxc-binding.h"
#include "jni-wrapper.h"

JNIEXPORT jstring JNICALL nativeGetVersion(JNIEnv *env, jclass clazz) {
    const char *version = lxc_get_version();
    return (*env)->NewStringUTF(env, version);
}

JNIEXPORT jobjectArray JNICALL nativeListContainers(JNIEnv *env, jclass clazz, jstring jlxcpath) {
    const char *lxcpath = (*env)->GetStringUTFChars(env, jlxcpath, NULL);
    char **names = NULL;
    struct lxc_container **containers = NULL;
    
    int count = list_defined_containers(lxcpath, &names, &containers);
    if (count <= 0) {
        (*env)->ReleaseStringUTFChars(env, jlxcpath, lxcpath);
        return NULL;
    }
    
    jclass stringClass = (*env)->FindClass(env, "java/lang/String");
    jobjectArray result = (*env)->NewObjectArray(env, count, stringClass, NULL);
    
    for (int i = 0; i < count; i++) {
        jstring name = (*env)->NewStringUTF(env, names[i]);
        (*env)->SetObjectArrayElement(env, result, i, name);
        (*env)->DeleteLocalRef(env, name);
        free(names[i]);
        lxc_container_put(containers[i]);
    }
    free(names);
    free(containers);
    
    (*env)->ReleaseStringUTFChars(env, jlxcpath, lxcpath);
    return result;
}

JNIEXPORT jboolean JNICALL nativeIsDefined(JNIEnv *env, jclass clazz, jstring jname, jstring jlxcpath) {
    const char *name = (*env)->GetStringUTFChars(env, jname, NULL);
    const char *lxcpath = (*env)->GetStringUTFChars(env, jlxcpath, NULL);
    
    struct lxc_container *c = lxc_container_new(name, lxcpath);
    jboolean result = JNI_FALSE;
    
    if (c) {
        result = jni_lxc_defined(c) ? JNI_TRUE : JNI_FALSE;
        lxc_container_put(c);
    }
    
    (*env)->ReleaseStringUTFChars(env, jname, name);
    (*env)->ReleaseStringUTFChars(env, jlxcpath, lxcpath);
    
    return result;
}

JNIEXPORT jboolean JNICALL nativeIsRunning(JNIEnv *env, jclass clazz, jstring jname, jstring jlxcpath) {
    const char *name = (*env)->GetStringUTFChars(env, jname, NULL);
    const char *lxcpath = (*env)->GetStringUTFChars(env, jlxcpath, NULL);
    
    struct lxc_container *c = lxc_container_new(name, lxcpath);
    jboolean result = JNI_FALSE;
    
    if (c) {
        result = jni_lxc_running(c) ? JNI_TRUE : JNI_FALSE;
        lxc_container_put(c);
    }
    
    (*env)->ReleaseStringUTFChars(env, jname, name);
    (*env)->ReleaseStringUTFChars(env, jlxcpath, lxcpath);
    
    return result;
}

JNIEXPORT jstring JNICALL nativeGetState(JNIEnv *env, jclass clazz, jstring jname, jstring jlxcpath) {
    const char *name = (*env)->GetStringUTFChars(env, jname, NULL);
    const char *lxcpath = (*env)->GetStringUTFChars(env, jlxcpath, NULL);
    
    struct lxc_container *c = lxc_container_new(name, lxcpath);
    jstring result = NULL;
    
    if (c) {
        const char *state = jni_lxc_state(c);
        if (state) {
            result = (*env)->NewStringUTF(env, state);
        }
        lxc_container_put(c);
    }
    
    (*env)->ReleaseStringUTFChars(env, jname, name);
    (*env)->ReleaseStringUTFChars(env, jlxcpath, lxcpath);
    
    return result;
}

JNIEXPORT jboolean JNICALL nativeStartContainer(JNIEnv *env, jclass clazz, jstring jname, jstring jlxcpath, jboolean juseInit) {
    const char *name = (*env)->GetStringUTFChars(env, jname, NULL);
    const char *lxcpath = (*env)->GetStringUTFChars(env, jlxcpath, NULL);
    
    struct lxc_container *c = lxc_container_new(name, lxcpath);
    jboolean result = JNI_FALSE;
    
    if (c) {
        int useinit = juseInit == JNI_TRUE ? 1 : 0;
        result = jni_lxc_start(c, useinit, NULL) ? JNI_TRUE : JNI_FALSE;
        lxc_container_put(c);
    }
    
    (*env)->ReleaseStringUTFChars(env, jname, name);
    (*env)->ReleaseStringUTFChars(env, jlxcpath, lxcpath);
    
    return result;
}

JNIEXPORT jboolean JNICALL nativeStopContainer(JNIEnv *env, jclass clazz, jstring jname, jstring jlxcpath) {
    const char *name = (*env)->GetStringUTFChars(env, jname, NULL);
    const char *lxcpath = (*env)->GetStringUTFChars(env, jlxcpath, NULL);
    
    struct lxc_container *c = lxc_container_new(name, lxcpath);
    jboolean result = JNI_FALSE;
    
    if (c) {
        result = jni_lxc_stop(c) ? JNI_TRUE : JNI_FALSE;
        lxc_container_put(c);
    }
    
    (*env)->ReleaseStringUTFChars(env, jname, name);
    (*env)->ReleaseStringUTFChars(env, jlxcpath, lxcpath);
    
    return result;
}

JNIEXPORT jboolean JNICALL nativeFreezeContainer(JNIEnv *env, jclass clazz, jstring jname, jstring jlxcpath) {
    const char *name = (*env)->GetStringUTFChars(env, jname, NULL);
    const char *lxcpath = (*env)->GetStringUTFChars(env, jlxcpath, NULL);
    
    struct lxc_container *c = lxc_container_new(name, lxcpath);
    jboolean result = JNI_FALSE;
    
    if (c) {
        result = jni_lxc_freeze(c) ? JNI_TRUE : JNI_FALSE;
        lxc_container_put(c);
    }
    
    (*env)->ReleaseStringUTFChars(env, jname, name);
    (*env)->ReleaseStringUTFChars(env, jlxcpath, lxcpath);
    
    return result;
}

JNIEXPORT jboolean JNICALL nativeUnfreezeContainer(JNIEnv *env, jclass clazz, jstring jname, jstring jlxcpath) {
    const char *name = (*env)->GetStringUTFChars(env, jname, NULL);
    const char *lxcpath = (*env)->GetStringUTFChars(env, jlxcpath, NULL);
    
    struct lxc_container *c = lxc_container_new(name, lxcpath);
    jboolean result = JNI_FALSE;
    
    if (c) {
        result = jni_lxc_unfreeze(c) ? JNI_TRUE : JNI_FALSE;
        lxc_container_put(c);
    }
    
    (*env)->ReleaseStringUTFChars(env, jname, name);
    (*env)->ReleaseStringUTFChars(env, jlxcpath, lxcpath);
    
    return result;
}

JNIEXPORT jboolean JNICALL nativeDestroyContainer(JNIEnv *env, jclass clazz, jstring jname, jstring jlxcpath) {
    const char *name = (*env)->GetStringUTFChars(env, jname, NULL);
    const char *lxcpath = (*env)->GetStringUTFChars(env, jlxcpath, NULL);
    
    struct lxc_container *c = lxc_container_new(name, lxcpath);
    jboolean result = JNI_FALSE;
    
    if (c) {
        result = jni_lxc_destroy(c) ? JNI_TRUE : JNI_FALSE;
        lxc_container_put(c);
    }
    
    (*env)->ReleaseStringUTFChars(env, jname, name);
    (*env)->ReleaseStringUTFChars(env, jlxcpath, lxcpath);
    
    return result;
}

JNIEXPORT jstring JNICALL nativeGetConfigItem(JNIEnv *env, jclass clazz, jstring jname, jstring jlxcpath, jstring jkey) {
    const char *name = (*env)->GetStringUTFChars(env, jname, NULL);
    const char *lxcpath = (*env)->GetStringUTFChars(env, jlxcpath, NULL);
    const char *key = (*env)->GetStringUTFChars(env, jkey, NULL);
    
    struct lxc_container *c = lxc_container_new(name, lxcpath);
    jstring result = NULL;
    
    if (c) {
        char *value = jni_lxc_get_config_item(c, key);
        if (value) {
            result = (*env)->NewStringUTF(env, value);
            free(value);
        }
        lxc_container_put(c);
    }
    
    (*env)->ReleaseStringUTFChars(env, jname, name);
    (*env)->ReleaseStringUTFChars(env, jlxcpath, lxcpath);
    (*env)->ReleaseStringUTFChars(env, jkey, key);
    
    return result;
}

JNIEXPORT jboolean JNICALL nativeSetConfigItem(JNIEnv *env, jclass clazz, jstring jname, jstring jlxcpath, jstring jkey, jstring jvalue) {
    const char *name = (*env)->GetStringUTFChars(env, jname, NULL);
    const char *lxcpath = (*env)->GetStringUTFChars(env, jlxcpath, NULL);
    const char *key = (*env)->GetStringUTFChars(env, jkey, NULL);
    const char *value = (*env)->GetStringUTFChars(env, jvalue, NULL);
    
    struct lxc_container *c = lxc_container_new(name, lxcpath);
    jboolean result = JNI_FALSE;
    
    if (c) {
        result = jni_lxc_set_config_item(c, key, value) ? JNI_TRUE : JNI_FALSE;
        lxc_container_put(c);
    }
    
    (*env)->ReleaseStringUTFChars(env, jname, name);
    (*env)->ReleaseStringUTFChars(env, jlxcpath, lxcpath);
    (*env)->ReleaseStringUTFChars(env, jkey, key);
    (*env)->ReleaseStringUTFChars(env, jvalue, value);
    
    return result;
}

JNIEXPORT jint JNICALL nativeCreateSnapshot(JNIEnv *env, jclass clazz, jstring jname, jstring jlxcpath) {
    const char *name = (*env)->GetStringUTFChars(env, jname, NULL);
    const char *lxcpath = (*env)->GetStringUTFChars(env, jlxcpath, NULL);
    
    struct lxc_container *c = lxc_container_new(name, lxcpath);
    jint result = -1;
    
    if (c) {
        result = jni_lxc_snapshot(c);
        lxc_container_put(c);
    }
    
    (*env)->ReleaseStringUTFChars(env, jname, name);
    (*env)->ReleaseStringUTFChars(env, jlxcpath, lxcpath);
    
    return result;
}

JNIEXPORT jobjectArray JNICALL nativeGetInterfaces(JNIEnv *env, jclass clazz, jstring jname, jstring jlxcpath) {
    const char *name = (*env)->GetStringUTFChars(env, jname, NULL);
    const char *lxcpath = (*env)->GetStringUTFChars(env, jlxcpath, NULL);
    
    struct lxc_container *c = lxc_container_new(name, lxcpath);
    jobjectArray result = NULL;
    
    if (c) {
        char **interfaces = jni_lxc_get_interfaces(c);
        if (interfaces) {
            int count = 0;
            while (interfaces[count] != NULL) {
                count++;
            }
            
            jclass stringClass = (*env)->FindClass(env, "java/lang/String");
            result = (*env)->NewObjectArray(env, count, stringClass, NULL);
            
            for (int i = 0; i < count; i++) {
                jstring iface = (*env)->NewStringUTF(env, interfaces[i]);
                (*env)->SetObjectArrayElement(env, result, i, iface);
                (*env)->DeleteLocalRef(env, iface);
                free(interfaces[i]);
            }
            free(interfaces);
        }
        lxc_container_put(c);
    }
    
    (*env)->ReleaseStringUTFChars(env, jname, name);
    (*env)->ReleaseStringUTFChars(env, jlxcpath, lxcpath);
    
    return result;
}

JNIEXPORT jint JNICALL nativeGetConsoleFd(JNIEnv *env, jclass clazz, jstring jname, jstring jlxcpath, jint jttynum) {
    const char *name = (*env)->GetStringUTFChars(env, jname, NULL);
    const char *lxcpath = (*env)->GetStringUTFChars(env, jlxcpath, NULL);
    int ttynum = jttynum;
    
    struct lxc_container *c = lxc_container_new(name, lxcpath);
    jint result = -1;
    
    if (c) {
        result = jni_lxc_console_getfd(c, ttynum);
        lxc_container_put(c);
    }
    
    (*env)->ReleaseStringUTFChars(env, jname, name);
    (*env)->ReleaseStringUTFChars(env, jlxcpath, lxcpath);
    
    return result;
}

JNIEXPORT jboolean JNICALL nativeConsole(JNIEnv *env, jclass clazz, jstring jname, jstring jlxcpath, 
                                         jint jttynum, jint jstdinfd, jint jstdoutfd, jint jstderrfd, jint jescape) {
    const char *name = (*env)->GetStringUTFChars(env, jname, NULL);
    const char *lxcpath = (*env)->GetStringUTFChars(env, jlxcpath, NULL);
    int ttynum = jttynum;
    int stdinfd = jstdinfd;
    int stdoutfd = jstdoutfd;
    int stderrfd = jstderrfd;
    int escape = jescape;
    
    struct lxc_container *c = lxc_container_new(name, lxcpath);
    jboolean result = JNI_FALSE;
    
    if (c) {
        result = jni_lxc_console(c, ttynum, stdinfd, stdoutfd, stderrfd, escape) ? JNI_TRUE : JNI_FALSE;
        lxc_container_put(c);
    }
    
    (*env)->ReleaseStringUTFChars(env, jname, name);
    (*env)->ReleaseStringUTFChars(env, jlxcpath, lxcpath);
    
    return result;
}

// Attach to container and run command (wait for completion)
JNIEXPORT jint JNICALL nativeAttachRunWait(JNIEnv *env, jclass clazz, jstring jname, jstring jlxcpath,
                                         jboolean jclearEnv, jint jnamespaces, jlong jpersonality,
                                         jint juid, jint jgid, jobjectArray jargv, jint jattachFlags) {
    const char *name = (*env)->GetStringUTFChars(env, jname, NULL);
    const char *lxcpath = (*env)->GetStringUTFChars(env, jlxcpath, NULL);
    
    struct lxc_container *c = lxc_container_new(name, lxcpath);
    jint result = -1;
    
    if (c) {
        bool clear_env = jclearEnv == JNI_TRUE;
        int namespaces = jnamespaces;
        long personality = jpersonality;
        uid_t uid = juid;
        gid_t gid = jgid;
        int attach_flags = jattachFlags;
        
        int argc = (*env)->GetArrayLength(env, jargv);
        const char **argv = NULL;
        if (argc > 0) {
            argv = (const char **)malloc(sizeof(char *) * (argc + 1));
            for (int i = 0; i < argc; i++) {
                jstring jarg = (*env)->GetObjectArrayElement(env, jargv, i);
                const char *arg = (*env)->GetStringUTFChars(env, jarg, NULL);
                argv[i] = arg;
                (*env)->ReleaseStringUTFChars(env, jarg, arg);
                (*env)->DeleteLocalRef(env, jarg);
            }
            argv[argc] = NULL;
        }
        
        lxc_groups_t groups = {0, NULL}; // Simplified handling, no group processing
        
        result = jni_lxc_attach_run_wait(c, clear_env, namespaces, personality, 
                                        uid, gid, groups, -1, -1, -1, 
                                        NULL, NULL, NULL, argv, attach_flags);
        
        if (argv) {
            free(argv);
        }
        lxc_container_put(c);
    }
    
    (*env)->ReleaseStringUTFChars(env, jname, name);
    (*env)->ReleaseStringUTFChars(env, jlxcpath, lxcpath);
    
    return result;
}

// Attach to container (do not wait)
JNIEXPORT jint JNICALL nativeAttachNoWait(JNIEnv *env, jclass clazz, jstring jname, jstring jlxcpath,
                                         jboolean jclearEnv, jint jnamespaces, jlong jpersonality,
                                         jint juid, jint jgid, jobjectArray jargv, jint jattachFlags) {
    const char *name = (*env)->GetStringUTFChars(env, jname, NULL);
    const char *lxcpath = (*env)->GetStringUTFChars(env, jlxcpath, NULL);
    
    struct lxc_container *c = lxc_container_new(name, lxcpath);
    jint result = -1;
    
    if (c) {
        bool clear_env = jclearEnv == JNI_TRUE;
        int namespaces = jnamespaces;
        long personality = jpersonality;
        uid_t uid = juid;
        gid_t gid = jgid;
        int attach_flags = jattachFlags;
        
        int argc = (*env)->GetArrayLength(env, jargv);
        const char **argv = NULL;
        if (argc > 0) {
            argv = (const char **)malloc(sizeof(char *) * (argc + 1));
            for (int i = 0; i < argc; i++) {
                jstring jarg = (*env)->GetObjectArrayElement(env, jargv, i);
                const char *arg = (*env)->GetStringUTFChars(env, jarg, NULL);
                argv[i] = arg;
                (*env)->ReleaseStringUTFChars(env, jarg, arg);
                (*env)->DeleteLocalRef(env, jarg);
            }
            argv[argc] = NULL;
        }
        
        lxc_groups_t groups = {0, NULL}; // Simplified handling, no group processing
        pid_t attached_pid;
        
        result = jni_lxc_attach_no_wait(c, clear_env, namespaces, personality, 
                                       uid, gid, groups, -1, -1, -1, 
                                       NULL, NULL, NULL, argv, &attached_pid, attach_flags);
        
        if (argv) {
            free(argv);
        }
        lxc_container_put(c);
    }
    
    (*env)->ReleaseStringUTFChars(env, jname, name);
    (*env)->ReleaseStringUTFChars(env, jlxcpath, lxcpath);
    
    return result;
}

JNIEXPORT jint JNICALL nativeGetErrorNum(JNIEnv *env, jclass clazz, jstring jname, jstring jlxcpath) {
    const char *name = (*env)->GetStringUTFChars(env, jname, NULL);
    const char *lxcpath = (*env)->GetStringUTFChars(env, jlxcpath, NULL);
    
    struct lxc_container *c = lxc_container_new(name, lxcpath);
    jint result = -1;
    
    if (c) {
        result = jni_lxc_error_num(c);
        lxc_container_put(c);
    }
    
    (*env)->ReleaseStringUTFChars(env, jname, name);
    (*env)->ReleaseStringUTFChars(env, jlxcpath, lxcpath);
    
    return result;
}

static JNINativeMethod method_table[] = {
    {"nativeGetVersion", "()Ljava/lang/String;", (void *)nativeGetVersion},
    {"nativeListContainers", "(Ljava/lang/String;)[Ljava/lang/String;", (void *)nativeListContainers},
    {"nativeIsDefined", "(Ljava/lang/String;Ljava/lang/String;)Z", (void *)nativeIsDefined},
    {"nativeIsRunning", "(Ljava/lang/String;Ljava/lang/String;)Z", (void *)nativeIsRunning},
    {"nativeGetState", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;", (void *)nativeGetState},
    {"nativeStartContainer", "(Ljava/lang/String;Ljava/lang/String;Z)Z", (void *)nativeStartContainer},
    {"nativeStopContainer", "(Ljava/lang/String;Ljava/lang/String;)Z", (void *)nativeStopContainer},
    {"nativeFreezeContainer", "(Ljava/lang/String;Ljava/lang/String;)Z", (void *)nativeFreezeContainer},
    {"nativeUnfreezeContainer", "(Ljava/lang/String;Ljava/lang/String;)Z", (void *)nativeUnfreezeContainer},
    {"nativeDestroyContainer", "(Ljava/lang/String;Ljava/lang/String;)Z", (void *)nativeDestroyContainer},
    {"nativeGetConfigItem", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;", (void *)nativeGetConfigItem},
    {"nativeSetConfigItem", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z", (void *)nativeSetConfigItem},
    {"nativeCreateSnapshot", "(Ljava/lang/String;Ljava/lang/String;)I", (void *)nativeCreateSnapshot},
    {"nativeGetInterfaces", "(Ljava/lang/String;Ljava/lang/String;)[Ljava/lang/String;", (void *)nativeGetInterfaces},
    {"nativeGetConsoleFd", "(Ljava/lang/String;Ljava/lang/String;I)I", (void *)nativeGetConsoleFd},
    {"nativeConsole", "(Ljava/lang/String;Ljava/lang/String;IIIII)Z", (void *)nativeConsole},
    {"nativeAttachRunWait", "(Ljava/lang/String;Ljava/lang/String;ZIJII[Ljava/lang/String;I)I", (void *)nativeAttachRunWait},
    {"nativeAttachNoWait", "(Ljava/lang/String;Ljava/lang/String;ZIJII[Ljava/lang/String;I)I", (void *)nativeAttachNoWait},
    {"nativeGetErrorNum", "(Ljava/lang/String;Ljava/lang/String;)I", (void *)nativeGetErrorNum},
};

static int register_native_methods(JNIEnv *env, const char *class_name, 
                                  JNINativeMethod *methods, int num_methods) {
    jclass clazz = (*env)->FindClass(env, class_name);
    if (clazz == NULL) {
        return JNI_FALSE;
    }
    
    if ((*env)->RegisterNatives(env, clazz, methods, num_methods) < 0) {
        return JNI_FALSE;
    }
    
    return JNI_TRUE;
}

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    jint result = -1;
    
    if ((*vm)->GetEnv(vm, (void **)&env, JNI_VERSION_1_6) != JNI_OK) {
        return result;
    }
    
    const char *class_name = "io/github/coap/lxc/LxcNative";
    int num_methods = sizeof(method_table) / sizeof(method_table[0]);
    
    if (!register_native_methods(env, class_name, method_table, num_methods)) {
        return result;
    }
    
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNI_OnUnload(JavaVM *vm, void *reserved) {
}