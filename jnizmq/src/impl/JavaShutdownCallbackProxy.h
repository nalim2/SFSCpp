#ifndef JNIZMQ_JAVASHUTDOWNCALLBACKPROXY_H
#define JNIZMQ_JAVASHUTDOWNCALLBACKPROXY_H

#include <zmqexecutorlib/Callback.h>
#include "JvmManager.h"

class JavaShutdownCallbackProxy : public Callback {
private:
    const std::string javaCallbackMethodName = "shutdownCallback";
    const std::string javaCallbackMethodSignature = "()V";
    jobject globalCallbackRef;
    jclass shutdownCallbackClass;
    jmethodID shutdownCallbackMethod;

public:
    JavaShutdownCallbackProxy(JNIEnv *env, jobject shutdownCallbackObject) {
        globalCallbackRef = env->NewGlobalRef(shutdownCallbackObject);
        shutdownCallbackClass = (jclass) env->NewGlobalRef(env->GetObjectClass(shutdownCallbackObject));
        shutdownCallbackMethod = env->GetMethodID(shutdownCallbackClass, javaCallbackMethodName.c_str(),
                                                  javaCallbackMethodSignature.c_str());
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

#endif //JNIZMQ_JAVASHUTDOWNCALLBACKPROXY_H
