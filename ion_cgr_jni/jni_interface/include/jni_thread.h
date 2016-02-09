/*
 * jni_thread.h
 *
 *  Created on: 24 nov 2015
 *      Author: michele
 */

#ifndef JNI_INCLUDE_JNI_THREAD_H_
#define JNI_INCLUDE_JNI_THREAD_H_

#include <pthread.h>
#include <jni.h>

typedef void * (*thread_function)(void*);
typedef struct {
	thread_function function;
	void * args;
} Passing;

int jni_thread_create(pthread_t *newthread, pthread_attr_t *attr,
		void *(*__start_routine) (void *), void * arg);
JNIEnv * getThreadLocalEnv();
void setThreadLocalEnv(JNIEnv * localEnv);

#endif /* JNI_INCLUDE_JNI_THREAD_H_ */
