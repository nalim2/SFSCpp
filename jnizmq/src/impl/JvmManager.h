#ifndef ZMQLOOPLIB_JVMMANAGER_H
#define ZMQLOOPLIB_JVMMANAGER_H

#include <jni.h>
#include <memory>
#include <functional>
#include <iostream>

namespace JvmStore {
    extern JavaVM *vm;
}

class JvmManager {
public:
    static JNIEnv *attachThread();
};


#endif //ZMQLOOPLIB_JVMMANAGER_H
