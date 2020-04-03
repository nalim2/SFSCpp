#include "src/headers/de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactor.h"
#include <zmqlooplib/ZmqExecutor.h>
#include "JvmManager.h"
#include "JavaInboxProxy.h"
#include "JavaShutdownCallbackProxy.h"

JNIEXPORT jlong JNICALL
Java_de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactor_initNative
        (JNIEnv *env, jclass, jclass inboxClass, jobject shutdownCallback) {
    env->GetJavaVM(&JvmStore::vm);
    jclass byteArrayClass = env->FindClass("[B");
    JvmStore::byteArrayClass = (jclass) env->NewGlobalRef(byteArrayClass);
    JvmStore::inboxClass = (jclass) env->NewGlobalRef(inboxClass);
    JvmStore::inboxMethod = env->GetMethodID(inboxClass, "addInboxMessage", "([[B)V");
    JvmStore::shutdownCallbackClass = (jclass) env->NewGlobalRef(env->GetObjectClass(shutdownCallback));
    JvmStore::shutdownCallbackMethod = env->GetMethodID(JvmStore::shutdownCallbackClass, "shutdownCallback", "()V");
    auto shutdownCallbackProxy = std::make_shared<JavaShutdownCallbackProxy>(shutdownCallback);
    ZmqExecutor *executor = ZmqExecutor::create(shutdownCallbackProxy);
    return (long) executor;
}

JNIEXPORT jlong JNICALL
Java_de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactor_createSubscriber
        (JNIEnv *env, jclass, jlong nativePointer, jobject inbox) {
    auto *executor = (ZmqExecutor *) nativePointer;
    auto inboxProxy = std::make_shared<JavaInboxProxy>(inbox);
    return (long) executor->createSubscriber(inboxProxy).get(); //todo wait for how long?
}

JNIEXPORT jlong JNICALL
Java_de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactor_createPublisher
        (JNIEnv *env, jclass, jlong nativePointer, jobject inbox) {
    auto *executor = (ZmqExecutor *) nativePointer;
    auto inboxProxy = std::make_shared<JavaInboxProxy>(inbox);
    return (long) executor->createPublisher(inboxProxy).get(); //todo wait for how long?
}

JNIEXPORT void JNICALL
Java_de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactor_close
        (JNIEnv *env, jclass, jlong nativePointer) {
    auto *executor = (ZmqExecutor *) nativePointer;
    delete executor;
    env->DeleteGlobalRef(JvmStore::inboxClass);
    env->DeleteGlobalRef(JvmStore::byteArrayClass);
    env->DeleteGlobalRef(JvmStore::shutdownCallbackClass);
    JvmStore::vm = nullptr;
    JvmStore::inboxMethod = nullptr;
    JvmStore::shutdownCallbackMethod = nullptr;
}

