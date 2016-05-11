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
const char * ONEtoION_interfaceClass = "cgr_jni/IONInterface";

pthread_key_t interfaceInfo_key;

struct InterfaceInfo_t {
	jobject currentMessage;
	Object outductList;
	int forwardResult;
};
typedef struct InterfaceInfo_t InterfaceInfo;

InterfaceInfo * interfaceInfo;


#ifndef CGR_DEBUG
#define CGR_DEBUG	0
#endif

#if CGR_DEBUG == 1
static void	printCgrTraceLine(void *data, unsigned int lineNbr,
			CgrTraceType traceType, ...)
{
	va_list args;
	const char *text;

	va_start(args, traceType);

	text = cgr_tracepoint_text(traceType);
	printf("NODE %ld: ", getNodeNum());
	vprintf(text, args);
	putchar('\n');

	va_end(args);
}
#endif


static InterfaceInfo * setInterfaceInfo(InterfaceInfo * interfaceInfo)
{
	if ((pthread_getspecific(interfaceInfo_key)) == NULL)
	{
		pthread_setspecific(interfaceInfo_key, interfaceInfo);
	}
	return interfaceInfo;
}
static InterfaceInfo * getInterfaceInfo()
{
	return pthread_getspecific(interfaceInfo_key);
}

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

static void updateMessageForfeitTime(jobject message, time_t forfeitTime)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass interfaceClass = (*jniEnv)->FindClass(jniEnv, ONEtoION_interfaceClass);
	time_t oneTime;
	oneTime = convertIonTimeToOne(forfeitTime);
	jmethodID method = (*jniEnv)->GetStaticMethodID(jniEnv, interfaceClass, "updateMessageForfeitTime","(Lcore/Message;J)V");
	(*jniEnv)->CallStaticVoidMethod(jniEnv, interfaceClass, method, message, oneTime);
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
 * outductName must be initialized.
 * Returns a pointer to outductName
 */
static char * getOutductName(jobject jOutduct, char * outductName)
{
	if (outductName == NULL)
		return NULL;
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass interfaceClass = (*jniEnv)->FindClass(jniEnv, ONEtoION_interfaceClass);
	jmethodID method = (*jniEnv)->GetStaticMethodID(jniEnv, interfaceClass, "getOutductName","(Lrouting/ContactGraphRouter$Outduct;)Ljava/lang/String;");
	jstring result = (*jniEnv)->CallStaticObjectMethod(jniEnv, interfaceClass, method, jOutduct);
	const char * nativeString = (*jniEnv)->GetStringUTFChars(jniEnv, result, NULL);
	strcpy(outductName, nativeString);
	(*jniEnv)->ReleaseStringUTFChars(jniEnv, result, nativeString);
	return outductName;
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

/**
 * Returns the total number of bytes already enqueued on this outduct
 */
static long getOutductTotalEnqueuedBytes(jobject jOutduct)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass interfaceClass = (*jniEnv)->FindClass(jniEnv, ONEtoION_interfaceClass);
	jmethodID method = (*jniEnv)->GetStaticMethodID(jniEnv, interfaceClass, "getOutductTotalEnququedBytes","(Lrouting/ContactGraphRouter$Outduct;)J");
	jlong result = (*jniEnv)->CallStaticLongMethod(jniEnv, interfaceClass, method, jOutduct);
	return (long) result;
}

static jobject cloneMessage(jobject jMessage)
{
	return NULL;
}

/**
 * Enqueues a message into an outduct
 */
static int insertBundleIntoOutduct(uvast localNodeNbr, jobject message, uvast toNodeNbr)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass interfaceClass = (*jniEnv)->FindClass(jniEnv, ONEtoION_interfaceClass);
	jmethodID method = (*jniEnv)->GetStaticMethodID(jniEnv, interfaceClass, "insertBundleIntoOutduct","(JLcore/Message;J)I");
	jint result = (*jniEnv)->CallStaticIntMethod(jniEnv, interfaceClass, method, localNodeNbr, message, toNodeNbr);
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
	jint result = (*jniEnv)->CallStaticIntMethod(jniEnv, interfaceClass, method, localNodeNbr, message);
	return (int) result;
}

static int getMessagePriority(jobject message)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass interfaceClass =(*jniEnv)->FindClass(jniEnv,ONEtoION_interfaceClass);
	jmethodID method = (*jniEnv)->GetStaticMethodID(jniEnv, interfaceClass, "getMessagePriority","(Lcore/Message;)I" );
	jint result = (*jniEnv)->CallStaticIntMethod(jniEnv,interfaceClass, method, message);
	return (int) result;
}

static long getOutductBulkBacklog(jobject jOutduct)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass interfaceClass = (*jniEnv)->FindClass(jniEnv, ONEtoION_interfaceClass);
	jmethodID method = (*jniEnv)->GetStaticMethodID(jniEnv, interfaceClass, "getOutductBulkBacklog","(Lrouting/ContactGraphRouter$Outduct;)J");
	jlong result = (*jniEnv)->CallStaticLongMethod(jniEnv, interfaceClass, method, jOutduct);
	return (long) result;
}

static long getOutductNormBacklog(jobject jOutduct)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass interfaceClass = (*jniEnv)->FindClass(jniEnv, ONEtoION_interfaceClass);
	jmethodID method = (*jniEnv)->GetStaticMethodID(jniEnv, interfaceClass, "getOutductNormalBacklog","(Lrouting/ContactGraphRouter$Outduct;)J");
	jlong result = (*jniEnv)->CallStaticLongMethod(jniEnv, interfaceClass, method, jOutduct);
	return (long) result;
}

static long getOutductExpBacklog(jobject jOutduct)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass interfaceClass = (*jniEnv)->FindClass(jniEnv, ONEtoION_interfaceClass);
	jmethodID method = (*jniEnv)->GetStaticMethodID(jniEnv, interfaceClass, "getOutductExpeditedBacklog","(Lrouting/ContactGraphRouter$Outduct;)J");
	jlong result = (*jniEnv)->CallStaticLongMethod(jniEnv, interfaceClass, method, jOutduct);
	return (long) result;
}

static int callManageOverbooking(uvast localNodeNbr,uvast proximateNodeNbr,double overbooked,double protect)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass interfaceClass =(*jniEnv)->FindClass(jniEnv,ONEtoION_interfaceClass);
	jmethodID method = (*jniEnv)->GetStaticMethodID(jniEnv, interfaceClass, "manageOverbooking","(JJDD)I" );
	jint result = (*jniEnv)->CallStaticIntMethod(jniEnv,interfaceClass, method, localNodeNbr,proximateNodeNbr,overbooked,protect);
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

	int pri = getMessagePriority(message);
	if(pri==1)
		 bundle->bundleProcFlags += 128;
	else if(pri==2)
		 bundle->bundleProcFlags += 256;

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
	long totEnqueued;
	char buf[MAX_CL_DUCT_NAME_LEN];
	memset(duct, 0, sizeof(Outduct));
	duct->blocked = isOutductBlocked(jOutduct);
	duct->maxPayloadLen = getMaxPayloadLen(jOutduct);
	//loadScalar(&(duct->stdBacklog), totEnqueued);
	strncpy(duct->name, getOutductName(jOutduct, buf), MAX_CL_DUCT_NAME_LEN);

	long bulkBacklog = getOutductBulkBacklog(jOutduct);
	long normBacklog = getOutductNormBacklog(jOutduct);
	long expBacklog = getOutductExpBacklog(jOutduct);
	loadScalar(&(duct->bulkBacklog), bulkBacklog);
	loadScalar(&(duct->stdBacklog), normBacklog);
	loadScalar(&(duct->urgentBacklog), expBacklog);

}

void init_ouduct_list()
{
	interfaceInfo->outductList = sdr_list_create(getIonsdr());
}
void wipe_outduct_list()
{
	Sdr sdr = getIonsdr();
	Object outductElt;
	Object outductObj;
	if (interfaceInfo->outductList != NULL)
	{
		outductElt = sdr_list_first(sdr, interfaceInfo->outductList);
		while (outductElt != NULL)
		{
			outductObj = sdr_list_data(sdr, outductElt);
			sdr_free(sdr, outductObj);
			outductElt = sdr_list_next(sdr, outductElt);
		}
		sdr_list_destroy(sdr, interfaceInfo->outductList, NULL, NULL);
	}
	interfaceInfo->outductList = NULL;
}

/**
 * get the outduct to nodeNbr
 * retrieves information from ONE runtime and sets directive->outductElt
 * plans should be NULL.
 * Returns 0 if no directive can be found.
 * Returns 1 if success
 */
int	getONEDirective(uvast nodeNbr, Object plans, Bundle *bundle,
			FwdDirective *directive)
{
	jobject jOutduct;
	Outduct outduct;
	Object outductObj;
	Object outductElt;
	jOutduct = getONEOutductToNode(getNodeNum(), nodeNbr);
	char outductName[MAX_CL_DUCT_NAME_LEN];
	if (jOutduct != NULL)
	{
		// init outduct list if not yet initialized
		if (interfaceInfo->outductList == NULL)
			init_ouduct_list();
		getOutductName(jOutduct, outductName);
		if ((outductElt = sdr_find(getIonsdr(), outductName, NULL)) == 0)
		{
			// convert java outduct object into ION Outduct struct
			ion_outduct(&outduct, jOutduct);
			// init sdr outduct object
			outductObj = sdr_malloc(getIonsdr(), sizeof(Outduct));
			sdr_write(getIonsdr(), outductObj, (char*)&outduct, sizeof(Outduct));
			// put outduct into sdr list
			outductElt = sdr_list_insert_first(getIonsdr(), interfaceInfo->outductList, outductObj);
			sdr_catlg(getIonsdr(), outductName, 0, outductElt);
		}
		directive->outductElt = outductElt;
		return 1;
	}
	return 0;
}

/**
 * Entry point for Contact Graph Router library
 * Tries to find the best route to terminusNodeNbr using libcgr.
 * If a feasible route is found, the bundle is enqueued into an outduct using bpEnqueONE().
 * If not, no operations are performed.
 * Returns the nodeNbr of the proximate node that the bundle has been enqueued to
 * or 0 if no proximate nodes have been found
 * or -1 in case of any error.
 */
int cgrForwardONE(jobject bundleONE, jlong terminusNodeNbr)
{
	Bundle *bundle;
	Object bundleObj;
	Object plans = (Object) 42; // this value will never be read but it is needed to pass the null check in cgr_forward()
	int result;
	CgrTrace * trace = NULL;
#if CGR_DEBUG == 1
	CgrTrace traceBuf;
	traceBuf.fn = printCgrTraceLine;
	traceBuf.data = NULL;
	trace = &traceBuf;
#endif
	interfaceInfo = malloc(sizeof(InterfaceInfo));
	interfaceInfo->forwardResult = 0;
	interfaceInfo->currentMessage = bundleONE;
	interfaceInfo->outductList = NULL;
	setInterfaceInfo(interfaceInfo);
	bundle = malloc(sizeof(Bundle));
	ion_bundle(bundle, bundleONE);
	bundleObj = sdr_malloc(getIonsdr(), sizeof(Bundle));
	sdr_write(getIonsdr(), bundleObj, (char*)bundle, sizeof(Bundle));
	result = cgr_forward(bundle, bundleObj, (uvast) terminusNodeNbr,
			plans, getONEDirective, trace);
	wipe_outduct_list();
	if (result >= 0)
		result = interfaceInfo->forwardResult;
	sdr_free(getIonsdr(), bundleObj);
	free(interfaceInfo);
	free(bundle);
	return result;
}

/**
 * Enqueues the bundle into an outduct.
 * This also update the bundle forfeit time.
 */
int bpEnqueONE(FwdDirective *directive, Bundle *bundle, Object bundleObj)
{
	uvast localNodeNbr, proximateNodeNbr;
	Object ductAddr;
	Outduct outduct;
	BpEvent forfeitEvent;
	sdr_read(getIonsdr(), (char*) &forfeitEvent, bundle->overdueElt, sizeof(BpEvent));
	updateMessageForfeitTime(interfaceInfo->currentMessage, forfeitEvent.time);
	localNodeNbr = getNodeNum();
	ductAddr = sdr_list_data(getIonsdr(), directive->outductElt);
	sdr_read(getIonsdr(), (char*)&outduct, ductAddr, sizeof(Outduct));
	proximateNodeNbr = atol(outduct.name);
	insertBundleIntoOutduct(localNodeNbr, interfaceInfo->currentMessage, proximateNodeNbr);
	interfaceInfo->forwardResult = proximateNodeNbr;
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
	interfaceInfo->currentMessage = message;
	bundle = malloc(sizeof(Bundle));
	ion_bundle(bundle, message);
	bundleObj = sdr_malloc(getIonsdr(), sizeof(Bundle));
	sdr_write(getIonsdr(), bundleObj, (char*) bundle, sizeof(Bundle));
	free(bundle);
	return 0;
}

int one_manage_overbooking(double overbooked,double protect,Bundle *lastSent)
{
	uvast localNodeNbr, proximateNodeNbr;
	int priority;

	priority= COS_FLAGS(lastSent->bundleProcFlags) & 0x03;
	if(priority==0 || overbooked == 0.0)
		return 0; //no overbooking

	localNodeNbr = getNodeNum();
	proximateNodeNbr = interfaceInfo->forwardResult;
	return callManageOverbooking(localNodeNbr,proximateNodeNbr,overbooked,protect);

}
