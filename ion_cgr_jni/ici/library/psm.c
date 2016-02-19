/*
 * psm.c
 *
 *  Created on: 03 nov 2015
 *      Author: michele
 */
#include "psm.h"

#include <stdlib.h>

#include "shared.h"
#include "jni_thread.h"

#define PsmPartitionClass "cgr_jni/psm/PsmPartition"

PsmAddress	Psm_zalloc(const char * s, int n,
		PsmPartition partition, unsigned long length)
{
	//cached for optimization
	static jmethodID method = 0;
	static jclass psmPartitionClass;
	JNIEnv * jniEnv = getThreadLocalEnv();
	void * pointer = (void *) malloc(length);
	if (method == 0)
	{
		psmPartitionClass =
				(*jniEnv)->FindClass(jniEnv, PsmPartitionClass);
		method = (*jniEnv)->GetMethodID(jniEnv,
				psmPartitionClass, "psmAlloc","(J)J");
	}
	jlong result = 	(*jniEnv)->CallLongMethod(jniEnv,
			partition, method, (jlong) pointer);
	if (pointer != (void*)(intptr_t) result)
		return NULL;
	return pointer;
}

void Psm_free(const char * s, int n,
		PsmPartition partition, PsmAddress address)
{
	//cached for optimization
	static jmethodID method = 0;
	static jclass psmPartitionClass;
	JNIEnv * jniEnv = getThreadLocalEnv();
	if (method == 0)
	{
		psmPartitionClass =
				(*jniEnv)->FindClass(jniEnv, PsmPartitionClass);
		method = (*jniEnv)->GetMethodID(jniEnv,
				psmPartitionClass, "psmFree","(J)V");
	}
	(*jniEnv)->CallVoidMethod(jniEnv, partition, method, address);
	free((void *)address);
}

int	psm_locate(PsmPartition partition , char *objName,
		PsmAddress *objLocation, PsmAddress *entryElt)
{
	//cached for optimization
	static jmethodID method = 0;
	static jclass psmPartitionClass;
	JNIEnv * jniEnv = getThreadLocalEnv();
	if (method == 0)
	{
		psmPartitionClass =
				(*jniEnv)->FindClass(jniEnv, PsmPartitionClass);
		method = (*jniEnv)->GetMethodID(jniEnv,
				psmPartitionClass, "psmLocate","(Ljava/lang/String;)J");
	}
	jstring name = (*jniEnv)->NewStringUTF(jniEnv, objName);
	jlong result = (*jniEnv)->CallLongMethod(jniEnv,
			partition, method, name);
	if (result < 0)
	{
		*entryElt = 0;
		return 0;
	}
	*objLocation = (PsmAddress) result;
	*entryElt = (PsmAddress) result;
	return 0;
}

int	Psm_catlg(const char * s, int n,
		PsmPartition partition, char *objName, PsmAddress objLocation)
{
	//cached for optimization
	static jmethodID method = 0;
	static jclass psmPartitionClass;
	JNIEnv * jniEnv = getThreadLocalEnv();
	if (method == 0)
	{
		psmPartitionClass =
				(*jniEnv)->FindClass(jniEnv, PsmPartitionClass);
		method = (*jniEnv)->GetMethodID(jniEnv,
				psmPartitionClass, "psmCatlg","(Ljava/lang/String;J)I");
	}
	jstring name = (*jniEnv)->NewStringUTF(jniEnv, objName);
	jint result = (*jniEnv)->CallIntMethod(jniEnv, partition,
			method, name, (jlong) objLocation);
	return result;
}

int	Psm_uncatlg(const char * s, int n,
		PsmPartition partition, char *objName)
{
	//cached for optimization
	static jmethodID method = 0;
	static jclass psmPartitionClass;
	JNIEnv * jniEnv = getThreadLocalEnv();
	if (method == 0)
	{
		psmPartitionClass =
				(*jniEnv)->FindClass(jniEnv, PsmPartitionClass);
		method = (*jniEnv)->GetMethodID(jniEnv,
				psmPartitionClass, "psmUncatlg","(Ljava/lang/String;)I");
	}
	jstring name = (*jniEnv)->NewStringUTF(jniEnv, objName);
	jint result = (*jniEnv)->CallIntMethod(jniEnv,
			partition, method, name);
	return result;
}

void * psp(PsmPartition partition, PsmAddress address)
{
	return (void *) address;
}

PsmAddress psa(PsmPartition partition , void * pointer)
{
	return (PsmAddress) pointer;
}
