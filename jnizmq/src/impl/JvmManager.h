#ifndef JNIZMQ_JVMMANAGER_H
#define JNIZMQ_JVMMANAGER_H

#include <jni.h>
#include <memory>
#include <functional>
#include <iostream>

namespace JvmStore {
    extern JavaVM *vm; //needs to be in cpp file to be unique for whole project
}

class JvmManager {
public:
    /**
     * Attaches this thread to the JVM. In case of being already attached, no-op. Autoamtically detaches thread on
     * thread end.
     * @return the JNIEnv for this thread
     */
    static JNIEnv *attachThread();
};


#endif //JNIZMQ_JVMMANAGER_H
