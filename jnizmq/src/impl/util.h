#ifndef JNIZMQ_UTIL_H
#define JNIZMQ_UTIL_H

#include <string>
#include <jni.h>

/**
 * Transforms Java String to native String (by copy).
 * @param env the JNIEnv for this thread
 * @param jstring the java string
 * @return the native string
 */
std::string jstringToString(JNIEnv *env, jstring jstring) {
    auto chars = env->GetStringUTFChars(jstring, nullptr);
    std::string cppString(chars); //make copy
    env->ReleaseStringUTFChars(jstring, chars);
    return cppString;
}

#endif //JNIZMQ_UTIL_H
