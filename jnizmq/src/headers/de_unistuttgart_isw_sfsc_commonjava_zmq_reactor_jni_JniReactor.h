/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactor */

#ifndef _Included_de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactor
#define _Included_de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactor
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactor
 * Method:    createNative
 * Signature: (Lde/unistuttgart/isw/sfsc/commonjava/zmq/reactor/jni/ShutdownCallback;)J
 */
JNIEXPORT jlong JNICALL Java_de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactor_createNative
  (JNIEnv *, jclass, jobject);

/*
 * Class:     de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactor
 * Method:    createSubscriber
 * Signature: (JLde/unistuttgart/isw/sfsc/commonjava/zmq/reactor/jni/InboxQueue;)J
 */
JNIEXPORT jlong JNICALL Java_de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactor_createSubscriber
  (JNIEnv *, jclass, jlong, jobject);

/*
 * Class:     de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactor
 * Method:    createPublisher
 * Signature: (JLde/unistuttgart/isw/sfsc/commonjava/zmq/reactor/jni/InboxQueue;)J
 */
JNIEXPORT jlong JNICALL Java_de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactor_createPublisher
  (JNIEnv *, jclass, jlong, jobject);

/*
 * Class:     de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactor
 * Method:    close
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_de_unistuttgart_isw_sfsc_commonjava_zmq_reactor_jni_JniReactor_close
  (JNIEnv *, jclass, jlong);

#ifdef __cplusplus
}
#endif
#endif
