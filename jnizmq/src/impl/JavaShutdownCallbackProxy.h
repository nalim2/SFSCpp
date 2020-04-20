#ifndef JNIZMQ_JAVASHUTDOWNCALLBACKPROXY_H
#define JNIZMQ_JAVASHUTDOWNCALLBACKPROXY_H

#include <zmqexecutorlib/Callback.h>
#include "JvmManager.h"

/**
 * Implementation of Callback for Java
 */
class JavaShutdownCallbackProxy : public Callback {
private:
    const std::string javaCallbackMethodName = "shutdownCallback";
    const std::string javaCallbackMethodSignature = "()V";
    jobject globalCallbackRef;
    jclass shutdownCallbackClass; //not sure how long the methodID stays valid, we just keep this to make sure
    jmethodID shutdownCallbackMethod;

public:
    /**
     * Constructor.
     * @param env the JNIEnv of this thread
     * @param shutdownCallbackObject Object whose "void shutdownCallback()" method will be called on shutdown
     */
    JavaShutdownCallbackProxy(JNIEnv *env, jobject shutdownCallbackObject) {
        globalCallbackRef = env->NewGlobalRef(shutdownCallbackObject);
        shutdownCallbackClass = (jclass) env->NewGlobalRef(env->GetObjectClass(shutdownCallbackObject));
        shutdownCallbackMethod = env->GetMethodID(shutdownCallbackClass, javaCallbackMethodName.c_str(),
                                                  javaCallbackMethodSignature.c_str());
    }

    /**
     * Performs callback. Can be called on arbitrary thread
     */
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
