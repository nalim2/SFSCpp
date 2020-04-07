#ifndef ZMQLOOPLIB_JAVASHUTDOWNCALLBACKPROXY_H
#define ZMQLOOPLIB_JAVASHUTDOWNCALLBACKPROXY_H

#include <zmqlooplib/Callback.h>
#include "JvmManager.h"

class JavaShutdownCallbackProxy : public Callback {
private:
    jobject globalCallbackRef;
    jclass shutdownCallbackClass;
    jmethodID shutdownCallbackMethod;

public:
    JavaShutdownCallbackProxy(JNIEnv *env, jobject shutdownCallbackObject) {
        globalCallbackRef = env->NewGlobalRef(shutdownCallbackObject);
        shutdownCallbackClass = (jclass) env->NewGlobalRef(env->GetObjectClass(shutdownCallbackObject));
        shutdownCallbackMethod = env->GetMethodID(shutdownCallbackClass, "shutdownCallback", "()V");
    }

    void callback() {
        JvmManager::attachThread()->CallVoidMethod(globalCallbackRef, shutdownCallbackMethod);
    };

    ~JavaShutdownCallbackProxy() {
        JNIEnv *env = JvmManager::attachThread();
        env->DeleteGlobalRef(globalCallbackRef);
        env->DeleteGlobalRef(shutdownCallbackClass);
    }
};

#endif //ZMQLOOPLIB_JAVASHUTDOWNCALLBACKPROXY_H
