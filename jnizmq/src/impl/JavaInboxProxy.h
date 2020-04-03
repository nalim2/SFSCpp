#ifndef ZMQLOOPLIB_JAVAINBOXPROXY_H
#define ZMQLOOPLIB_JAVAINBOXPROXY_H

#include <jni.h>
#include <memory>
#include <zmqpp/message.hpp>
#include <zmqlooplib/Inbox.h>
#include "JvmManager.h"


class JavaInboxProxy : public Inbox {
private:
    jobject globalInboxRef;
public:
    JavaInboxProxy(jobject inboxRef) {
        globalInboxRef = JvmManager::attachThread()->NewGlobalRef(inboxRef);
    }

    void receive(std::unique_ptr<zmqpp::message> message) {
        JNIEnv *env = JvmManager::attachThread();
        int outerLength = message->parts();
        jobjectArray outer = env->NewObjectArray(outerLength, JvmStore::byteArrayClass, nullptr);
        for (int i = 0; i < outerLength; i++) {
            int innerLength = message->size(i);
            jbyteArray inner = env->NewByteArray(innerLength);
            env->SetByteArrayRegion(inner, 0, innerLength, (jbyte *) message->raw_data(i));
            env->SetObjectArrayElement(outer, i, inner);
        }
        env->CallVoidMethod(globalInboxRef, JvmStore::inboxMethod, outer);
    };

    ~JavaInboxProxy() {
        JvmManager::attachThread()->DeleteGlobalRef(globalInboxRef);
    }
};

#endif //ZMQLOOPLIB_JAVAINBOXPROXY_H
