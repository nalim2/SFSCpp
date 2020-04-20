#ifndef JNIZMQ_JAVAINBOXPROXY_H
#define JNIZMQ_JAVAINBOXPROXY_H

#include <jni.h>
#include <memory>
#include <zmqpp/message.hpp>
#include <zmqexecutorlib/Inbox.h>
#include "JvmManager.h"


class JavaInboxProxy : public Inbox {
private:
    const std::string javaAddMethodName = "addInboxMessage";
    const std::string javaAddMethodSignature = "([[B)V";
    const std::string javaByteArrayClassName = "[B";
    jobject globalInboxRef;
    jclass inboxClass; //not sure how long the methodID stays valid, we just keep this to make sure
    jmethodID inboxMethod;
    jclass byteArrayClass;

public:
    /**
     * Constructor.
     * @param env the JNIEnv of this thread
     * @param inboxObject Object whose "void addInboxMessage(byte[][] message)" method will be called on shutdown
     */
    JavaInboxProxy(JNIEnv *env, jobject inboxObject) {
        globalInboxRef = env->NewGlobalRef(inboxObject);
        inboxClass = (jclass) env->NewGlobalRef(env->GetObjectClass(inboxObject));
        inboxMethod = env->GetMethodID(inboxClass, javaAddMethodName.c_str(), javaAddMethodSignature.c_str());
        byteArrayClass = (jclass) env->NewGlobalRef(env->FindClass(javaByteArrayClassName.c_str()));
    }

    /**
     * Receives message and passes its bytes to the java method.  Can be called on arbitrary thread.
     *
     * If JVM supports it, without copy.
     * See https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/functions.html
     * Set<PrimitiveType>ArrayRegion Routines.
     *
     * @param message the message containing the bytes to pass
     */
    void receive(std::unique_ptr<zmqpp::message> message) {
        JNIEnv *env = JvmManager::attachThread();
        int outerLength = message->parts(); //amount of message parts
        jobjectArray outer = env->NewObjectArray(outerLength, byteArrayClass, nullptr);
        for (int i = 0; i < outerLength; i++) {
            int innerLength = message->size(i); //size of part
            jbyteArray inner = env->NewByteArray(innerLength); //allocate correct size
            env->SetByteArrayRegion(inner, 0, innerLength, (jbyte *) message->raw_data(i)); //copy/move
            env->SetObjectArrayElement(outer, i, inner); //set inner into outer array
        }
        env->CallVoidMethod(globalInboxRef, inboxMethod, outer); //perform method call and pass the data
    };

    ~JavaInboxProxy() {
        JNIEnv *env = JvmManager::attachThread();
        env->DeleteGlobalRef(globalInboxRef);
        env->DeleteGlobalRef(inboxClass);
        env->DeleteGlobalRef(byteArrayClass);
    }
};

#endif //JNIZMQ_JAVAINBOXPROXY_H
