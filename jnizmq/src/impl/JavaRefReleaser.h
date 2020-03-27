#ifndef ZMQLOOPLIB_JAVAREFRELEASER_H
#define ZMQLOOPLIB_JAVAREFRELEASER_H

#include <jni.h>
#include <memory>
#include <functional>
#include "JvmManager.h"

class JavaRefReleaser {

private:
    jobject ref;

public:
    JavaRefReleaser(jobject ref) : ref(ref) {}

    static void release(void *, void *hint) {
        auto releaser = (JavaRefReleaser *) hint;
        releaser->release();
        delete releaser;
    }

private:
    void release() {
        JvmManager::attachThread()->DeleteGlobalRef(ref);
    }

};


#endif //ZMQLOOPLIB_JAVAREFRELEASER_H
