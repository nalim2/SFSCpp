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
    auto outer = (jobjectArray) env->NewGlobalRef(array);
    auto messageSupplier = std::make_unique<std::function<zmqpp::message()>>([outer]() {
        JNIEnv *env = JvmManager::attachThread();
        zmqpp::message message;
        int length = env->GetArrayLength(outer);
        for (int i = 0; i < length; i++) {
            auto inner = (jbyteArray) env->GetObjectArrayElement(outer, i);
            auto globalInner = (jbyteArray) env->NewGlobalRef(inner);
            int innerLength = env->GetArrayLength(
                    globalInner); //since its byte, number of elements is number of bytes
            message.add_nocopy(globalInner, innerLength, &JavaRefReleaser::release,
                               new JavaRefReleaser(globalInner));
        }
        env->DeleteGlobalRef(outer);
        return message;
    });
    socket->send(std::move(messageSupplier));

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

