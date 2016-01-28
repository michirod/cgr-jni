/*
 *  cgr_Libcgr.c
 *
 *  Created on: 07 dic 2015
 *      Author: michele
 */

#include <jni.h>
#include <stdio.h>

#include "ion.h"
#include "ionadmin.h"
#include "platform.h"
#include "shared.h"
#include "init_global.h"
#include "normal_c.h"
#include "ONEtoION_interface.h"

JNIEXPORT jint JNICALL Java_cgr_Libcgr_initializeNode(JNIEnv *env, jclass thisObj, jint nodeNum)
{
	jint result;
	if (javaVM == NULL)
		(*env)->GetJavaVM(env, &javaVM);
	init_global();
	setThreadLocalEnv(env);
	JNIEnv * jniEnv = getThreadLocalEnv();
	uvast unodeNum = (uvast) nodeNum;
	setNodeNum(unodeNum);
	result = ionInitialize(NULL, unodeNum);
	if (result == 0)
		result = cgr_start();
	else result = -10;
	fflush(stdout);
	return result;
}

JNIEXPORT jint JNICALL Java_cgr_Libcgr_readContactPlan(JNIEnv *env, jclass thisObj, jint nodeNum, jstring fileName)
{
	jint result;
	if (javaVM == NULL)
		(*env)->GetJavaVM(env, &javaVM);
	init_global();
	setThreadLocalEnv(env);
	JNIEnv * jniEnv = getThreadLocalEnv();
	uvast unodeNum = (uvast) nodeNum;
	setNodeNum(unodeNum);
	const char *nativeString = (*env)->GetStringUTFChars(env, fileName, 0);
	result = runIonadmin(nativeString);
	fflush(stdout);
	return result;
}

JNIEXPORT jint JNICALL Java_cgr_Libcgr_processLine(JNIEnv *env, jclass thisObj, jint nodeNum, jstring line)
{
	jint result;
	if (javaVM == NULL)
		(*env)->GetJavaVM(env, &javaVM);
	init_global();
	setThreadLocalEnv(env);
	JNIEnv * jniEnv = getThreadLocalEnv();
	uvast unodeNum = (uvast) nodeNum;
	setNodeNum(unodeNum);
	const char *nativeString = (*env)->GetStringUTFChars(env, line, 0);
	result = processLine(nativeString, strlen(nativeString));
	fflush(stdout);
	return result;
}

JNIEXPORT jint JNICALL Java_cgr_Libcgr_cgrForward(JNIEnv *env, jclass thisObj, jint nodeNum, jobject message, jlong terminusNodeNbr)
{
	jint result;
	if (javaVM == NULL)
		(*env)->GetJavaVM(env, &javaVM);
	init_global();
	setThreadLocalEnv(env);
	JNIEnv * jniEnv = getThreadLocalEnv();
	uvast unodeNum = (uvast) nodeNum;
	setNodeNum(unodeNum);
	result = cgrForwardONE(message, terminusNodeNbr);
	return result;
}

JNIEXPORT jint JNICALL Java_cgr_Libcgr_genericTest  (JNIEnv *env, jclass thisObj, jint nodeNum, jobject message)
{
	jint result;
	if (javaVM == NULL)
		(*env)->GetJavaVM(env, &javaVM);
	init_global();
	setThreadLocalEnv(env);
	JNIEnv * jniEnv = getThreadLocalEnv();
	uvast unodeNum = (uvast) nodeNum;
	setNodeNum(unodeNum);
	result = testMessage(message);
	return result;
}
