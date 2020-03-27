#include "JvmManager.h"

namespace JvmStore {
    JavaVM *vm;
    jclass byteArrayClass;
    jclass inboxClass;
    jclass exceptionCallbackClass;
    jmethodID inboxMethod;
    jmethodID exceptionCallbackMethod;
}

JNIEnv *JvmManager::attachThread() {
    JNIEnv *env;
    JvmStore::vm->AttachCurrentThread((void **) &env, nullptr);
    return env;
}

void JvmManager::detachThread() {
    JvmStore::vm->DetachCurrentThread();
}
