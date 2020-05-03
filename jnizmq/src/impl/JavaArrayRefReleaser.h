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

    /**
     * Method to release the ByteArrayElement obtained with GetByteArrayElements.
     * @param hint pointer to the Releaser object
     */
    static void release(void *, void *hint) {
        auto releaser = (JavaArrayRefReleaser *) hint;
        releaser->release();
        delete releaser;
    }

private:
    void release() {
        //JNI_ABORT for no copy back of changes since we wont change anything
        JvmManager::attachThread()->ReleaseByteArrayElements(jArray, nativeArray, JNI_ABORT);
    }

};

#endif //JNIZMQ_JAVAARRAYREFRELEASER_H
