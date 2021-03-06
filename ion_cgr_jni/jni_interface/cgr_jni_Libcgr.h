/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class cgr_jni_Libcgr */

#ifndef _Included_cgr_jni_Libcgr
#define _Included_cgr_jni_Libcgr
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     cgr_jni_Libcgr
 * Method:    initializeNode
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_cgr_1jni_Libcgr_initializeNode
  (JNIEnv *, jclass, jint);

/*
 * Class:     cgr_jni_Libcgr
 * Method:    finalizeNode
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_cgr_1jni_Libcgr_finalizeNode
  (JNIEnv *, jclass, jint);

/*
 * Class:     cgr_jni_Libcgr
 * Method:    readContactPlan
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_cgr_1jni_Libcgr_readContactPlan
  (JNIEnv *, jclass, jint, jstring);

/*
 * Class:     cgr_jni_Libcgr
 * Method:    processLine
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_cgr_1jni_Libcgr_processLine
  (JNIEnv *, jclass, jint, jstring);

/*
 * Class:     cgr_jni_Libcgr
 * Method:    cgrForward
 * Signature: (ILcore/Message;J)I
 */
JNIEXPORT jint JNICALL Java_cgr_1jni_Libcgr_cgrForward
  (JNIEnv *, jclass, jint, jobject, jlong);

/*
 * Class:     cgr_jni_Libcgr
 * Method:    genericTest
 * Signature: (ILcore/Message;)I
 */
JNIEXPORT jint JNICALL Java_cgr_1jni_Libcgr_genericTest
  (JNIEnv *, jclass, jint, jobject);

#ifdef __cplusplus
}
#endif
#endif
