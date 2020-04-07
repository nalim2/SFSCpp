#include "JvmManager.h"

namespace JvmStore {
    JavaVM *vm;
}

JNIEnv *JvmManager::attachThread() {
    JNIEnv *env;
    JvmStore::vm->AttachCurrentThread((void **) &env, nullptr);
    return env;
}

void JvmManager::detachThread() {
    JvmStore::vm->DetachCurrentThread();
}
