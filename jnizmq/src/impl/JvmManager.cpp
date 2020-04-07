#include "JvmManager.h"

namespace JvmStore {
    JavaVM *vm;
}

JNIEnv *JvmManager::attachThread() {
    JNIEnv *env;
    JvmStore::vm->AttachCurrentThread((void **) &env, nullptr);
    struct DetachOnJniExit {
        ~DetachOnJniExit() {
            JvmStore::vm->DetachCurrentThread();
        }
    };
    thread_local DetachOnJniExit detach;
    return env;
}