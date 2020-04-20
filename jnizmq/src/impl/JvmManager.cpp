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
    //thread_local (implicit static), so once for every thread who called this. Deallocated automatically on thread end
    thread_local DetachOnJniExit detach;
    return env;
}