#ifndef ZMQLOOPLIB_JAVAINBOXPROXY_H
#define ZMQLOOPLIB_JAVAINBOXPROXY_H

#include <jni.h>
#include <memory>
#include <zmqpp/message.hpp>
#include <zmqlooplib/Inbox.h>
#include "JvmManager.h"


class JavaInboxProxy : public Inbox {
private:
    const std::string javaAddMethodName = "addInboxMessage";
    const std::string javaAddMethodSignature = "([[B)V";
    const std::string javaByteArrayClassName = "[B";
    jobject globalInboxRef;
    jclass inboxClass;
    jmethodID inboxMethod;
    jclass byteArrayClass;

public:
    JavaInboxProxy(JNIEnv *env, jobject inboxObject) {
        globalInboxRef = env->NewGlobalRef(inboxObject);
        inboxClass = (jclass) env->NewGlobalRef(env->GetObjectClass(inboxObject));
        inboxMethod = env->GetMethodID(inboxClass, javaAddMethodName.c_str(), javaAddMethodSignature.c_str());
        byteArrayClass = (jclass) env->NewGlobalRef(env->FindClass(javaByteArrayClassName.c_str()));
    }

    void receive(std::unique_ptr<zmqpp::message> message) {
        JNIEnv *env = JvmManager::attachThread();
        int outerLength = message->parts();
        jobjectArray outer = env->NewObjectArray(outerLength, byteArrayClass, nullptr);
        for (int i = 0; i < outerLength; i++) {
            int innerLength = message->size(i);
            jbyteArray inner = env->NewByteArray(innerLength);
            env->SetByteArrayRegion(inner, 0, innerLength, (jbyte *) message->raw_data(i));
            env->SetObjectArrayElement(outer, i, inner);
        }
        env->CallVoidMethod(globalInboxRef, inboxMethod, outer);
    };

    ~JavaInboxProxy() {
        JNIEnv *env = JvmManager::attachThread();
        env->DeleteGlobalRef(globalInboxRef);
        env->DeleteGlobalRef(inboxClass);
        env->DeleteGlobalRef(byteArrayClass);
    }
};

#endif //ZMQLOOPLIB_JAVAINBOXPROXY_H
