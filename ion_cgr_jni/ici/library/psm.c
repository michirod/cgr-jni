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


PsmAddress	Psm_zalloc(const char * s, int n, PsmPartition partition, unsigned long length)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	void * pointer = (void *) malloc(length);
	jclass psmPartitionClass = (*jniEnv)->FindClass(jniEnv, PsmPartitionClass);
	jmethodID zalloc = (*jniEnv)->GetMethodID(jniEnv, psmPartitionClass, "psmAlloc","(J)J");
	jlong result = (*jniEnv)->CallLongMethod(jniEnv, partition, zalloc, (jlong) pointer);
	if (pointer != (void*)(intptr_t) result)
		return NULL;
	return pointer;
}

void Psm_free(const char * s, int n, PsmPartition partition, PsmAddress address)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass psmPartitionClass = (*jniEnv)->FindClass(jniEnv, PsmPartitionClass);
	jmethodID method = (*jniEnv)->GetMethodID(jniEnv, psmPartitionClass, "psmFree","(J)V");
	(*jniEnv)->CallVoidMethod(jniEnv, partition, method, address);
	free((void *)address);
}

int	psm_locate(PsmPartition partition , char *objName,	PsmAddress *objLocation, PsmAddress *entryElt)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass psmPartitionClass = (*jniEnv)->FindClass(jniEnv, PsmPartitionClass);
	jstring name = (*jniEnv)->NewStringUTF(jniEnv, objName);
	//jclass psmAddressClass = (*jniEnv)->FindClass(jniEnv, PsmAddressClass);
	//jmethodID constructor = (*jniEnv)->GetMethodID(jniEnv, psmAddressClass, "<init>","()V");
	//*objLocation = (*jniEnv)->NewObject(jniEnv, psmAddressClass, constructor);
	//*entryElt = (*jniEnv)->NewObject(jniEnv, psmAddressClass, constructor);
	//jmethodID locate = (*jniEnv)->GetMethodID(jniEnv, psmPartitionClass, "psmLocate","(Ljava/lang/String;Ljni/test/psm/PsmAddress;Ljni/test/psm/PsmAddress;)I");
	//jint result = (*jniEnv)->CallIntMethod(jniEnv, partition, locate, name, *objLocation, *entryElt);
	jmethodID locate = (*jniEnv)->GetMethodID(jniEnv, psmPartitionClass, "psmLocate","(Ljava/lang/String;)J");
	jlong result = (*jniEnv)->CallLongMethod(jniEnv, partition, locate, name);
	if (result < 0)
	{
		*entryElt = 0;
		return 0;
	}
	*objLocation = (PsmAddress) result;
	*entryElt = (PsmAddress) result;
	return 0;
}

int	Psm_catlg(const char * s, int n, PsmPartition partition, char *objName, PsmAddress objLocation)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass psmPartitionClass = (*jniEnv)->FindClass(jniEnv, PsmPartitionClass);
	//jmethodID catlg = (*jniEnv)->GetMethodID(jniEnv, psmPartitionClass, "psmCatlg","(Ljava/lang/String;Ljni/test/psm/PsmAddress;)I");
	jmethodID catlg = (*jniEnv)->GetMethodID(jniEnv, psmPartitionClass, "psmCatlg","(Ljava/lang/String;J)I");
	jstring name = (*jniEnv)->NewStringUTF(jniEnv, objName);
	jint result = (*jniEnv)->CallIntMethod(jniEnv, partition, catlg, name, (jlong) objLocation);
	return result;
}

int	Psm_uncatlg(const char * s, int n, PsmPartition partition, char *objName)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass psmPartitionClass = (*jniEnv)->FindClass(jniEnv, PsmPartitionClass);
	jmethodID uncatlg = (*jniEnv)->GetMethodID(jniEnv, psmPartitionClass, "psmUncatlg","(Ljava/lang/String;)I");
	jstring name = (*jniEnv)->NewStringUTF(jniEnv, objName);
	jint result = (*jniEnv)->CallIntMethod(jniEnv, partition, uncatlg, name);
	return result;
}

void * psp(PsmPartition partition, PsmAddress address)
{
	//JNIEnv * jniEnv = getThreadLocalEnv();
	//jclass psmAddressClass = (*jniEnv)->FindClass(jniEnv, PsmAddressClass);
	//jmethodID getPointer = (*jniEnv)->GetMethodID(jniEnv, psmAddressClass, "getPointer","()J");
	//jlong pointer = (*jniEnv)->CallLongMethod(jniEnv, address, getPointer);
	//return (void *) pointer;
	return (void *) address;
}

PsmAddress psa(PsmPartition partition , void * pointer)
{
	//JNIEnv * jniEnv = getThreadLocalEnv();
	//jclass psmPartitionClass = (*jniEnv)->FindClass(jniEnv, PsmPartitionClass);
	//jmethodID getAddress = (*jniEnv)->GetMethodID(jniEnv, psmPartitionClass, "getAddress","(J)Ljni/test/psm/PsmAddress;");
	//jobject result = (*jniEnv)->CallObjectMethod(jniEnv, partition, getAddress, pointer);
	//return (PsmAddress) result;
	return (PsmAddress) pointer;
}
