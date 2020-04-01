#include "src/headers/de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactiveSocket.h"
#include "util.h"
#include "JavaRefReleaser.h"
#include "JvmManager.h"

#include <zmqlooplib/Socket.h>
#include <zmqpp/message.hpp>


JNIEXPORT void JNICALL
Java_de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactiveSocket_add
        (JNIEnv *env, jclass, jlong nativePointer, jobjectArray array) {
    auto *socket = (Socket *) nativePointer;
    auto jOuter = (jobjectArray) env->NewGlobalRef(array);
    auto messageSupplier = std::make_shared<std::function<zmqpp::message()>>([jOuter]() {
        JNIEnv *env = JvmManager::attachThread();
        zmqpp::message message;
        int length = env->GetArrayLength(jOuter);
        for (int i = 0; i < length; i++) {
            auto jInner = (jbyteArray) env->GetObjectArrayElement(jOuter, i);
            int innerLength = env->GetArrayLength(
                    jInner); //since its byte, number of elements is number of bytes
            auto innerNative = env->GetByteArrayElements(jInner, nullptr);
            message.add_nocopy(innerNative, innerLength, &JavaRefReleaser::release,
                               new JavaRefReleaser(jInner, innerNative));
        }
        env->DeleteGlobalRef(jOuter);
        return message;
    });
    socket->send(messageSupplier);
}

JNIEXPORT void JNICALL
Java_de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactiveSocket_connect
        (JNIEnv *env, jclass, jlong nativePointer, jstring jAddress) {
    auto *socket = (Socket *) nativePointer;
    std::string address = jstringToString(env, jAddress);
    socket->connect(address);
}

JNIEXPORT void JNICALL
Java_de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactiveSocket_disconnect
        (JNIEnv *env, jclass, jlong nativePointer, jstring jAddress) {
    auto *socket = (Socket *) nativePointer;
    std::string address = jstringToString(env, jAddress);
    socket->disconnect(address);
}

JNIEXPORT void JNICALL
Java_de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactiveSocket_bind
        (JNIEnv *env, jclass, jlong nativePointer, jstring jAddress) {
    auto *socket = (Socket *) nativePointer;
    std::string address = jstringToString(env, jAddress);
    socket->bind(address);
}

JNIEXPORT void JNICALL
Java_de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactiveSocket_unbind
        (JNIEnv *env, jclass, jlong nativePointer, jstring jAddress) {
    auto *socket = (Socket *) nativePointer;
    std::string address = jstringToString(env, jAddress);
    socket->unbind(address);
}

JNIEXPORT void JNICALL
Java_de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactiveSocket_setXPubVerbose
        (JNIEnv *, jclass, jlong nativePointer) {
    auto *socket = (Socket *) nativePointer;
    socket->setXPubVerbose();
}

JNIEXPORT void JNICALL
Java_de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactiveSocket_shutdownNative
        (JNIEnv *, jclass, jlong nativePointer) {
    auto *socket = (Socket *) nativePointer;
    delete socket;
}

