/*
 * ONEtoION_interface.c
 *
 *  Created on: 17 dic 2015
 *      Author: michele
 */

#include <jni.h>

#include "bpP.h"
#include "cgr.h"
#include "shared.h"
#include "utils.h"
#include "init_global.h"

const char * jMessageClass = "core/Message";
const char * jOuductClass = "routing/ContactGraphRouting$Outduct";
const char * ONEtoION_interfaceClass = "cgr/IONInterface";
jobject currentMessage;
Object outductList = NULL;

static uvast getMessageSenderNbr(jobject message)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass interfaceClass = (*jniEnv)->FindClass(jniEnv, ONEtoION_interfaceClass);
	jmethodID method = (*jniEnv)->GetStaticMethodID(jniEnv, interfaceClass, "getMessageSenderNbr","(Lcore/Message;)J");
	jlong result = (*jniEnv)->CallStaticLongMethod(jniEnv, interfaceClass, method, message);
	return (uvast) result;
}
static uvast getMessageDestinationNbr(jobject message)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass interfaceClass = (*jniEnv)->FindClass(jniEnv, ONEtoION_interfaceClass);
	jmethodID method = (*jniEnv)->GetStaticMethodID(jniEnv, interfaceClass, "getMessageDestinationNbr","(Lcore/Message;)J");
	jlong result = (*jniEnv)->CallStaticLongMethod(jniEnv, interfaceClass, method, message);
	return (uvast) result;
}
static unsigned int getMessageCreationTime(jobject message)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass interfaceClass = (*jniEnv)->FindClass(jniEnv, ONEtoION_interfaceClass);
	jmethodID method = (*jniEnv)->GetStaticMethodID(jniEnv, interfaceClass, "getMessageCreationTime","(Lcore/Message;)J");
	jlong result = (*jniEnv)->CallStaticLongMethod(jniEnv, interfaceClass, method, message);
	return (uvast) result + getONEReferenceTime();
}
/**
 * return message time to live (sec)
 */
static unsigned int getMessageTTL(jobject message)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass interfaceClass = (*jniEnv)->FindClass(jniEnv, ONEtoION_interfaceClass);
	jmethodID method = (*jniEnv)->GetStaticMethodID(jniEnv, interfaceClass, "getMessageTTL","(Lcore/Message;)J");
	jlong result = (*jniEnv)->CallStaticLongMethod(jniEnv, interfaceClass, method, message);
	return (uvast) result;
}
/**
 * retrun bundle payload size
 */
static unsigned int getMessageSize(jobject message)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass interfaceClass = (*jniEnv)->FindClass(jniEnv, ONEtoION_interfaceClass);
	jmethodID method = (*jniEnv)->GetStaticMethodID(jniEnv, interfaceClass, "getMessageSize","(Lcore/Message;)J");
	jlong result = (*jniEnv)->CallStaticLongMethod(jniEnv, interfaceClass, method, message);
	return (uvast) result;
}
/**
 * return true if the outduct is blocked (in ONE this should return always false)
 */
static bool_t isOutductBlocked(jobject jOutduct)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass interfaceClass = (*jniEnv)->FindClass(jniEnv, ONEtoION_interfaceClass);
	jmethodID method = (*jniEnv)->GetStaticMethodID(jniEnv, interfaceClass, "isOutductBlocked","(Lrouting/ContactGraphRouter$Outduct;)Z");
	jboolean result = (*jniEnv)->CallStaticBooleanMethod(jniEnv, interfaceClass, method, jOutduct);
	return (bool_t) result;
}
/**
 * return the outduct name
 * in ONE, the outduct name is a property of the class Outduct
 * and the name is the nodeNbr associated to the outduct.
 * User must free the result string after use.
 *
 */
static char * getOutductName(jobject jOutduct)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass interfaceClass = (*jniEnv)->FindClass(jniEnv, ONEtoION_interfaceClass);
	jmethodID method = (*jniEnv)->GetStaticMethodID(jniEnv, interfaceClass, "getOutductName","(Lrouting/ContactGraphRouter$Outduct;)Ljava/lang/String;");
	jstring result = (*jniEnv)->CallStaticObjectMethod(jniEnv, interfaceClass, method, jOutduct);
	const char * nativeString = (*jniEnv)->GetStringUTFChars(jniEnv, result, NULL);
	char * string = malloc(strlen(nativeString) + 1);
	strcpy(string, nativeString);
	(*jniEnv)->ReleaseStringUTFChars(jniEnv, result, nativeString);
	return string;
}
/**
 * arbitrary defined
 */
static unsigned int getMaxPayloadLen(jobject jOutduct)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass interfaceClass = (*jniEnv)->FindClass(jniEnv, ONEtoION_interfaceClass);
	jmethodID method = (*jniEnv)->GetStaticMethodID(jniEnv, interfaceClass, "getMaxPayloadLen","(Lrouting/ContactGraphRouter$Outduct;)I");
	jint result = (*jniEnv)->CallStaticIntMethod(jniEnv, interfaceClass, method, jOutduct);
	return (uvast) result;
}
/**
 * return the java Outduct object of the node localNodeNbr to the node toNodeNbr
 */
static jobject getONEOutductToNode(uvast localNodeNbr, uvast toNodeNbr)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass interfaceClass = (*jniEnv)->FindClass(jniEnv, ONEtoION_interfaceClass);
	jmethodID method = (*jniEnv)->GetStaticMethodID(jniEnv, interfaceClass, "getONEOutductToNode","(JJ)Lrouting/ContactGraphRouter$Outduct;");
	jobject result = (*jniEnv)->CallStaticObjectMethod(jniEnv, interfaceClass, method, localNodeNbr, toNodeNbr);
	return result;
}

static jobject cloneMessage(jobject jMessage)
{
	return NULL;
}

/**
 * insert a message into an outduct
 */
static int insertBundleIntoOutduct(uvast localNodeNbr, jobject message, uvast toNodeNbr)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass interfaceClass = (*jniEnv)->FindClass(jniEnv, ONEtoION_interfaceClass);
	jmethodID method = (*jniEnv)->GetStaticMethodID(jniEnv, interfaceClass, "insertBundleIntoOutduct","(JLcore/Message;J)I");
	jint result = (*jniEnv)->CallStaticObjectMethod(jniEnv, interfaceClass, method, localNodeNbr, message, toNodeNbr);
	return (int) result;
}
/**
 * insert a message into local limbo
 */
static int insertBundleIntoLimbo(uvast localNodeNbr, jobject message)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass interfaceClass = (*jniEnv)->FindClass(jniEnv, ONEtoION_interfaceClass);
	jmethodID method = (*jniEnv)->GetStaticMethodID(jniEnv, interfaceClass, "insertBundleIntoLimbo","(JLcore/Message;J)I");
	jint result = (*jniEnv)->CallStaticObjectMethod(jniEnv, interfaceClass, method, localNodeNbr, message);
	return (int) result;
}

/**
 * Convert a java Message object to an ION Bundle
 */
void ion_bundle(Bundle * bundle, jobject message)
{
	memset(bundle, 0, sizeof(Bundle));
	bundle->returnToSender = 1;
	bundle->clDossier.senderNodeNbr = getMessageSenderNbr(message);
	//bundle->expirationTime = getMessageCreationTime(message) + getMessageTTL(message);
	bundle->expirationTime = getSimulatedUTCTime() + getMessageTTL(message);
	bundle->destination.c.nodeNbr = getMessageDestinationNbr(message);
	bundle->destination.c.serviceNbr = 0;
	bundle->destination.cbhe = 1;
	bundle->payload.length = getMessageSize(message);
	bundle->bundleProcFlags = BDL_DOES_NOT_FRAGMENT;
	bundle->extendedCOS.ordinal = 0;
	bundle->extendedCOS.flags = 0;
	bundle->dictionaryLength = 0;
	bundle->extensionsLength[PRE_PAYLOAD] = 0;
	bundle->extensionsLength[POST_PAYLOAD] = 0;
}

/**
 * Convert a java Outduct object into an ION Outduct
 */
void ion_outduct(Outduct * duct, jobject jOutduct)
{
	memset(duct, 0, sizeof(Outduct));
	strcpy(duct->name, getOutductName(jOutduct));
	duct->blocked = isOutductBlocked(jOutduct);
	duct->maxPayloadLen = getMaxPayloadLen(jOutduct);
	strncpy(duct->name, getOutductName(jOutduct), MAX_CL_DUCT_NAME_LEN);
}

void init_ouduct_list()
{
	outductList = sdr_list_create(getIonsdr());
}

/**
 * get the outduct to nodeNbr
 * plans should be NULL.
 * Return 0 if no directive can be found.
 * directive->outductElt must be sdr_freed after use.
 */
int	getONEDirective(uvast nodeNbr, Object plans, Bundle *bundle,
			FwdDirective *directive)
{
	jobject jOutduct;
	Outduct * outduct;
	Object outductObj;
	Object outductElt;
	jOutduct = getONEOutductToNode(getNodeNum(), nodeNbr);
	if (jOutduct != NULL)
	{
		// init outduct list if not yet initialized
		if (outductList == NULL)
			init_ouduct_list();
		// allocate memory for outduct. Must be freed when done.
		outduct = malloc(sizeof(Outduct));
		// convert java outduct object into ION Outduct struct
		ion_outduct(outduct, jOutduct);
		// init sdr outduct object
		outductObj = sdr_malloc(getIonsdr(), sizeof(Outduct));
		sdr_write(getIonsdr(), outductObj, (char*)outduct, sizeof(Outduct));
		// put outduct into sdr list
		outductElt = sdr_list_insert_first(getIonsdr(), outductList, outductObj);
		directive->outductElt = outductElt;
		return 1;
	}
	return 0;
}

int cgrForwardONE(jobject bundleONE, jlong terminusNodeNbr)
{
	Bundle *bundle;
	Object bundleObj;
	Object plans = (Object) 42; // this value will never be read but it is needed to pass the null check in cgr_forward()
	currentMessage = bundleONE;
	bundle = malloc(sizeof(Bundle));
	ion_bundle(bundle, bundleONE);
	bundleObj = sdr_malloc(getIonsdr(), sizeof(Bundle));
	sdr_write(getIonsdr(), bundleObj, (char*)bundle, sizeof(Bundle));
	return cgr_forward(bundle, bundleObj, (uvast) terminusNodeNbr, plans, getONEDirective, NULL);
}

int bpEnqueONE(FwdDirective *directive, Bundle *bundle, Object bundleObj)
{
	uvast localNodeNbr, proximateNodeNbr;
	Outduct outduct;
	localNodeNbr = getNodeNum();
	sdr_read(getIonsdr(), (char*)&outduct, directive->outductElt, sizeof(Outduct));
	proximateNodeNbr = atol(outduct.name);
	insertBundleIntoOutduct(localNodeNbr, currentMessage, proximateNodeNbr);
	return 0;
}

int bpCloneONE(Bundle *oldBundle, Bundle *newBundle, Object *newBundleObj)
{
	return 0;
}

int testMessage(jobject message)
{
	Bundle *bundle;
	Object bundleObj;
	currentMessage = message;
	bundle = malloc(sizeof(Bundle));
	ion_bundle(bundle, message);
	bundleObj = sdr_malloc(getIonsdr(), sizeof(Bundle));
	sdr_write(getIonsdr(), bundleObj, (char*) bundle, sizeof(Bundle));
	free(bundle);
	return 0;
}

int testOuduct(jobject jOutduct)
{
	Outduct outduct;
	Object outductObj;
	Object outductElt;
	ion_outduct(&outduct, jOutduct);
	outductObj = sdr_malloc(getIonsdr(), sizeof(Outduct));
	sdr_write(getIonsdr(), outductObj, &outduct, sizeof(Outduct));
	outductElt = sdr_list_insert_first(getIonsdr(), outductList, outductObj);
	return 0;
}
