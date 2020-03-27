#ifndef ZMQLOOPLIB_UTIL_H
#define ZMQLOOPLIB_UTIL_H

#include <string>
#include <jni.h>

std::string jstringToString(JNIEnv *env, jstring jstring) {
    auto chars = env->GetStringUTFChars(jstring, nullptr);
    std::string cppString(chars);
    env->ReleaseStringUTFChars(jstring, chars);
    return cppString;
}

#endif //ZMQLOOPLIB_UTIL_H
