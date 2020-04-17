#ifndef JNIZMQ_JAVAARRAYREFRELEASER_H
#define JNIZMQ_JAVAARRAYREFRELEASER_H

#include <jni.h>
#include <memory>
#include <functional>
#include "JvmManager.h"

class JavaArrayRefReleaser {

private:
    jbyteArray jArray;
    jbyte *nativeArray;

public:
    JavaArrayRefReleaser(jbyteArray jArray, jbyte *nativeArray) : jArray(jArray), nativeArray(nativeArray) {}

    static void release(void *, void *hint) {
        auto releaser = (JavaArrayRefReleaser *) hint;
        releaser->release();
        delete releaser;
    }

private:
    void release() {
        JvmManager::attachThread()->ReleaseByteArrayElements(jArray, nativeArray, JNI_ABORT);
    }

};

#endif //JNIZMQ_JAVAARRAYREFRELEASER_H
