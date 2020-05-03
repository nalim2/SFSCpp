#include "src/headers/de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactiveSocket.h"
#include "util.h"
#include "JavaArrayRefReleaser.h"
#include "JvmManager.h"

#include <zmqexecutorlib/Socket.h>
#include <zmqpp/message.hpp>


JNIEXPORT void JNICALL
Java_de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactiveSocket_add
        (JNIEnv *env, jclass, jlong nativePointer, jobjectArray array) {
    auto *socket = (Socket *) nativePointer;
    auto jOuter = (jobjectArray) env->NewGlobalRef(array); //global ref since we need access in another thread later
    // will be executed in an arbitrary thread
    auto messageSupplier = std::make_shared<std::function<zmqpp::message()>>([jOuter]() {
        JNIEnv *env = JvmManager::attachThread(); //since this can be run on arbitrary thread, first attach
        zmqpp::message message;
        int length = env->GetArrayLength(jOuter);
        for (int i = 0; i < length; i++) {
            auto jInner = (jbyteArray) env->GetObjectArrayElement(jOuter, i);
            int innerLength = env->GetArrayLength(jInner);

            /*
             * We dont use GetByteArrayElementsCritical() because the send() by zmq happens async, we dont know when,
             * and the GetxCritical version is... well its too critical to be used that async.
             * See docs https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/functions.html
             */
            auto innerNative = env->GetByteArrayElements(jInner, nullptr);

            /*
             * Add data to the message without copy (uses underlying bytes of added object). Automatically release with
             * Releaser after success.
             *
             * innerLength: since we deal with byte arrays, number of elements == number of bytes
             */
            message.add_nocopy(innerNative, innerLength, &JavaArrayRefReleaser::release,
                               new JavaArrayRefReleaser(jInner, innerNative));
        }
        env->DeleteGlobalRef(jOuter); //delete global ref we created in the beginning
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

