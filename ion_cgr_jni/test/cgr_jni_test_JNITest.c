/*
 * JNITest.c
 *
 *  Created on: 28 ott 2015
 *      Author: michele
 */

#include "cgr_jni_test_JNITest.h"

#include <jni.h>
#include <stdio.h>

#include "shared.h"
#include "init_global.h"
#include "normal_c.h"
/**
 * Java passa di qua quando chiama metodi native
 * Nei metodi mi salvo il riferimento alla JVM (la uso poi in list.c)
 * Faccio qualche conversione e invoco i metodi delle librerie C
 */


JNIEXPORT jint JNICALL Java_cgr_1jni_test_JNITest_doSomething(JNIEnv *env, jobject thisObj, jlong nodeNum, jstring string)
{
	init_global();
	if (javaVM == NULL)
		(*env)->GetJavaVM(env, &javaVM);
	setThreadLocalEnv(env);
	const char *nativeString = (*env)->GetStringUTFChars(env, string, 0);
	return doSomething(nodeNum, (char *)nativeString);
}

JNIEXPORT jint JNICALL Java_cgr_1jni_test_JNITest_doSomethingWithLists(JNIEnv *env, jobject thisObj, jlong nodeNum, jobjectArray array)
{
	init_global();
	if (javaVM == NULL)
		(*env)->GetJavaVM(env, &javaVM);
	setThreadLocalEnv(env);
	jsize len = (*env)->GetArrayLength(env, array);
	char * strings[len];
	int i;
	for (i = 0; i < len; i++)
	{
		jstring string = (*env)->GetObjectArrayElement(env, array, i);
		const char *nativeString = (*env)->GetStringUTFChars(env, string, 0);
		strings[i] = nativeString;
	}
	return doSomethingWithLists(nodeNum, len, strings);
}

JNIEXPORT jint JNICALL Java_cgr_1jni_test_JNITest_doSomethingWithPsm(JNIEnv *env, jobject thisObj, jlong nodeNum, jobjectArray array)
{
	init_global();
	if (javaVM == NULL)
		(*env)->GetJavaVM(env, &javaVM);
	setThreadLocalEnv(env);
	jsize len = (*env)->GetArrayLength(env, array);
	char * strings[len];
	int i;
	for (i = 0; i < len; i++)
	{
		jstring string = (*env)->GetObjectArrayElement(env, array, i);
		const char *nativeString = (*env)->GetStringUTFChars(env, string, 0);
		strings[i] = nativeString;
	}
	return doSomethingWithPsm(nodeNum, len, strings);
}
