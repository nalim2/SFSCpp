#ifndef ZMQLOOPLIB_JAVAREFRELEASER_H
#define ZMQLOOPLIB_JAVAREFRELEASER_H

#include <jni.h>
#include <memory>
#include <functional>
#include "JvmManager.h"

class JavaRefReleaser {

private:
    jbyteArray jArray;
    jbyte *nativeArray;

public:
    JavaRefReleaser(jbyteArray jArray, jbyte *nativeArray) : jArray(jArray), nativeArray(nativeArray) {}

    static void release(void *, void *hint) {
        auto releaser = (JavaRefReleaser *) hint;
        releaser->release();
        delete releaser;
    }

private:
    void release() {
        JvmManager::attachThread()->ReleaseByteArrayElements(jArray, nativeArray, JNI_ABORT);
    }

};


#endif //ZMQLOOPLIB_JAVAREFRELEASER_H
