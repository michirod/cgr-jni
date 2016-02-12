/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class cgr_jni_Libocgr */

#ifndef _Included_cgr_jni_Libocgr
#define _Included_cgr_jni_Libocgr
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     cgr_jni_Libocgr
 * Method:    predictContacts
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_cgr_1jni_Libocgr_predictContacts
  (JNIEnv *, jclass, jint);

/*
 * Class:     cgr_jni_Libocgr
 * Method:    exchangeCurrentDiscoveredContatcs
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_cgr_1jni_Libocgr_exchangeCurrentDiscoveredContatcs
  (JNIEnv *, jclass, jint, jint);

/*
 * Class:     cgr_jni_Libocgr
 * Method:    exchangeContactHistory
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_cgr_1jni_Libocgr_exchangeContactHistory
  (JNIEnv *, jclass, jint, jint);

/*
 * Class:     cgr_jni_Libocgr
 * Method:    contactDiscoveryAquired
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_cgr_1jni_Libocgr_contactDiscoveryAquired
  (JNIEnv *, jclass, jint, jint, jint);

/*
 * Class:     cgr_jni_Libocgr
 * Method:    contactDiscoveryLost
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_cgr_1jni_Libocgr_contactDiscoveryLost
  (JNIEnv *, jclass, jint, jint);

#ifdef __cplusplus
}
#endif
#endif
