#include "JvmManager.h"

namespace JvmStore {
    JavaVM *vm;
    jclass byteArrayClass;
    jclass inboxClass;
    jmethodID inboxMethod;
    jclass shutdownHandlerClass;
    jmethodID shutdownHandlerMethod;
}

JNIEnv *JvmManager::attachThread() {
    JNIEnv *env;
    JvmStore::vm->AttachCurrentThread((void **) &env, nullptr);
    return env;
}

void JvmManager::detachThread() {
    JvmStore::vm->DetachCurrentThread();
}
