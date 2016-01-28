/*
 * shared.h
 *
 *  Created on: 28 ott 2015
 *      Author: michele
 */

#ifndef JNI_SHARED_H_
#define JNI_SHARED_H_
#include <jni.h>
#include <pthread.h>

#include "jni_thread.h"

/**
 * Questa variabile globale rappresenta il Java Runtime (la VM)
 * L'ho messo qua, cosi' recupero il riferimento una volta per tutte.
 */

//extern JNIEnv * jniEnv;
extern JavaVM * javaVM;

extern int initialized;
extern pthread_key_t nodeNum_key;
extern pthread_key_t jniEnv_key;

#endif /* JNI_SHARED_H_ */
