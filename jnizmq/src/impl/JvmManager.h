#ifndef ZMQLOOPLIB_JVMMANAGER_H
#define ZMQLOOPLIB_JVMMANAGER_H

#include <jni.h>
#include <memory>
#include <functional>

namespace JvmStore {
    extern JavaVM *vm;
    extern jclass byteArrayClass;
    extern jclass inboxClass;
    extern jmethodID inboxMethod;
    extern jclass shutdownHandlerClass;
    extern jmethodID shutdownHandlerMethod;
}

class JvmManager {
public:

    static JNIEnv *attachThread();

    static void detachThread();

    static void detachThreadAdapter(void *) { detachThread(); }
};

thread_local static std::unique_ptr<void *, decltype(&JvmManager::detachThreadAdapter)>
        detacher(nullptr, JvmManager::detachThreadAdapter);


#endif //ZMQLOOPLIB_JVMMANAGER_H