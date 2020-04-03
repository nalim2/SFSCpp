#ifndef ZMQLOOPLIB_JAVASHUTDOWNCALLBACKPROXY_H
#define ZMQLOOPLIB_JAVASHUTDOWNCALLBACKPROXY_H

#include <zmqlooplib/Callback.h>
#include "JvmManager.h"

class JavaShutdownCallbackProxy : public Callback {
private:
    jobject globalCallbackRef;
public:
    JavaShutdownCallbackProxy(jobject callbackRef) {
        globalCallbackRef = JvmManager::attachThread()->NewGlobalRef(callbackRef);
    }

    void callback() {
        JvmManager::attachThread()->CallVoidMethod(globalCallbackRef, JvmStore::shutdownCallbackMethod);
    };

    ~JavaShutdownCallbackProxy() {
        JvmManager::attachThread()->DeleteGlobalRef(globalCallbackRef);
    }
};

#endif //ZMQLOOPLIB_JAVASHUTDOWNCALLBACKPROXY_H
