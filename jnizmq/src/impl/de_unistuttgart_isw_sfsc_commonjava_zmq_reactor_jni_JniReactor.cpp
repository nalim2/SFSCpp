#include "src/headers/de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactor.h"
#include <zmqexecutorlib/ZmqExecutor.h>
#include "JvmManager.h"
#include "JavaInboxProxy.h"
#include "JavaShutdownCallbackProxy.h"

JNIEXPORT jlong JNICALL
Java_de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactor_createNative
        (JNIEnv *env, jclass, jobject shutdownCallbackObject) {
    env->GetJavaVM(&JvmStore::vm);
    auto shutdownCallbackProxy = std::make_shared<JavaShutdownCallbackProxy>(env, shutdownCallbackObject);
    auto executor = ZmqExecutor::create(shutdownCallbackProxy);
    return (long) executor;
}

JNIEXPORT jlong JNICALL
Java_de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactor_createSubscriber
        (JNIEnv *env, jclass, jlong nativePointer, jobject inboxObject) {
    auto *executor = (ZmqExecutor *) nativePointer;
    auto inboxProxy = std::make_shared<JavaInboxProxy>(env, inboxObject);
    return (long) executor->createSubscriber(inboxProxy).get();
}

JNIEXPORT jlong JNICALL
Java_de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactor_createPublisher
        (JNIEnv *env, jclass, jlong nativePointer, jobject inboxObject) {
    auto *executor = (ZmqExecutor *) nativePointer;
    auto inboxProxy = std::make_shared<JavaInboxProxy>(env, inboxObject);
    return (long) executor->createPublisher(inboxProxy).get();
}

JNIEXPORT void JNICALL
Java_de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactor_close
        (JNIEnv *, jclass, jlong nativePointer) {
    auto *executor = (ZmqExecutor *) nativePointer;
    delete executor;
}

