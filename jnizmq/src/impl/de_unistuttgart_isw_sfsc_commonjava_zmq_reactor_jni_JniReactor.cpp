#include "src/headers/de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactor.h"
#include <zmqlooplib/ZmqExecutor.h>
#include "JvmManager.h"
#include "JavaInboxProxy.h"

JNIEXPORT jlong JNICALL
Java_de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactor_initNative
        (JNIEnv *env, jclass, jclass inboxClass, jobject shutdownHandler) {
    env->GetJavaVM(&JvmStore::vm);
    jclass byteArrayClass = env->FindClass("[B"); //todo nullchecks for jnizmq stuff needed?
    JvmStore::byteArrayClass = (jclass) env->NewGlobalRef(byteArrayClass);
    JvmStore::inboxClass = (jclass) env->NewGlobalRef(inboxClass);
    JvmStore::inboxMethod = env->GetMethodID(inboxClass, "addInboxMessage", "([[B)V");
    JvmStore::shutdownHandlerClass = (jclass) env->NewGlobalRef(env->GetObjectClass(shutdownHandler));
    JvmStore::shutdownHandlerMethod = env->GetMethodID(JvmStore::shutdownHandlerClass, "shutdownEvent", "()V");
    ZmqExecutor *executor = ZmqExecutor::create(); //todo use shutdwonHandler
    return (long) executor;
}

JNIEXPORT jlong JNICALL
Java_de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactor_createSubscriber
        (JNIEnv *env, jclass, jlong nativePointer, jobject inbox) {
    auto *executor = (ZmqExecutor *) nativePointer;
    auto globalInboxRef = (jobject) env->NewGlobalRef(inbox);
    auto inboxProxy = std::make_shared<JavaInboxProxy>(globalInboxRef);
    return (long) executor->createSubscriber(std::static_pointer_cast<Inbox>(inboxProxy)).get(); //todo wait for how long?
}

JNIEXPORT jlong JNICALL
Java_de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactor_createPublisher
        (JNIEnv * env, jclass, jlong nativePointer, jobject inbox) {
    auto *executor = (ZmqExecutor *) nativePointer;
    auto globalInboxRef = (jobject) env->NewGlobalRef(inbox);
    auto inboxProxy = std::make_shared<JavaInboxProxy>(globalInboxRef);
    return (long) executor->createPublisher(std::static_pointer_cast<Inbox>(inboxProxy)).get(); //todo wait for how long?
}

JNIEXPORT void JNICALL
Java_de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactor_close
        (JNIEnv *, jclass, jlong nativePointer) {
    auto *executor = (ZmqExecutor *) nativePointer;
    delete executor;
}

