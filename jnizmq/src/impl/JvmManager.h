#ifndef JNIZMQ_JVMMANAGER_H
#define JNIZMQ_JVMMANAGER_H

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


#endif //JNIZMQ_JVMMANAGER_H
