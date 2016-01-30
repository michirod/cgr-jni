/*
 * utils.c
 *
 *  Created on: 04 nov 2015
 *      Author: michele
 */

#include "utils.h"

#include "ion.h"
#include "sdr.h"
#include "shared.h"
#include "init_global.h"

PsmPartition getIonPsmPartition(long nodeNum, int partNum)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass psmPartitionManagerClass = (*jniEnv)->FindClass(jniEnv, PsmPartitionManagerClass);
	jmethodID method = (*jniEnv)->GetStaticMethodID(jniEnv, psmPartitionManagerClass, "getPartition","(JI)Ljni/test/psm/PsmPartition;");
	jobject partition = (*jniEnv)->CallStaticObjectMethod(jniEnv, psmPartitionManagerClass, method, nodeNum, partNum);
	if (partition == NULL)
	{
		method = (*jniEnv)->GetStaticMethodID(jniEnv, psmPartitionManagerClass, "newPartition","(JI)Ljni/test/psm/PsmPartition;");
		partition = (*jniEnv)->CallStaticObjectMethod(jniEnv, psmPartitionManagerClass, method, nodeNum, partNum);
	}
	return (PsmPartition) partition;
}

PsmPartition newIonPsmPartition(long nodeNum, int partNum)
{
	JNIEnv * jniEnv = getThreadLocalEnv();
	jclass psmPartitionManagerClass = (*jniEnv)->FindClass(jniEnv, PsmPartitionManagerClass);
	jmethodID method = (*jniEnv)->GetStaticMethodID(jniEnv, psmPartitionManagerClass, "newPartition","(JI)Ljni/test/psm/PsmPartition;");
	jobject partition = (*jniEnv)->CallStaticObjectMethod(jniEnv, psmPartitionManagerClass, method, nodeNum, partNum);
	return (PsmPartition) partition;
}

IonDB * createIonDb(Sdr ionsdr, IonDB * iondbPtr)
{
	if (iondbPtr == NULL)
		iondbPtr = (IonDB*) malloc(sizeof(IonDB));
	#define iondbBuf (*iondbPtr)
	memset((char *) &iondbBuf, 0, sizeof(IonDB));
	iondbBuf.ownNodeNbr = getNodeNum();
	iondbBuf.productionRate = -1;	/*	Unknown.	*/
	iondbBuf.consumptionRate = -1;	/*	Unknown.	*/
	//limit = (sdr_heap_size(ionsdr) / 100) * (100 - ION_SEQUESTERED);

	/*	By default, let outbound ZCOs occupy up to
	 *	half of the available heap space, leaving
	 *	the other half for inbound ZCO acquisition.	*/

	//zco_set_max_heap_occupancy(ionsdr, limit/2, ZcoInbound);
	//zco_set_max_heap_occupancy(ionsdr, limit/2, ZcoOutbound);

	/*	By default, the occupancy ceiling is 50% more
	 *	than the outbound ZCO allocation.		*/

	//iondbBuf.occupancyCeiling = zco_get_max_file_occupancy(ionsdr, ZcoOutbound);
	//iondbBuf.occupancyCeiling += (limit/4);
	iondbBuf.contacts = sdr_list_create(ionsdr);
	iondbBuf.ranges = sdr_list_create(ionsdr);
	iondbBuf.maxClockError = 0;
	iondbBuf.clockIsSynchronized = 1;
    //memcpy(&iondbBuf.parmcopy, parms, sizeof(IonParms));
	return iondbPtr;
}

IonVdb * createIonVdb(char * ionvdbName)
{
	IonVdb	*vdb = NULL;
	PsmAddress	vdbAddress;
	PsmAddress	elt;
	Sdr		sdr;
	PsmPartition	ionwm;
	IonDB		iondb;
	char * name = ionvdbName;

	/*	Attaching to volatile database.			*/

	ionwm = getIonwm();
	if (psm_locate(ionwm, name, &vdbAddress, &elt) < 0)
	{
		putErrmsg("Failed searching for vdb.", name);
		return NULL;
	}

	if (elt)
	{
		vdb = (IonVdb *) psp(ionwm, vdbAddress);
	}

	if (vdb != NULL)
		return vdb;
	/*	ION volatile database doesn't exist yet.	*/

	sdr = getIonsdr();
	CHKNULL(sdr_begin_xn(sdr));	/*	To lock memory.	*/
	vdbAddress = psm_zalloc(ionwm, sizeof(IonVdb));
	if (vdbAddress == 0)
	{
		sdr_exit_xn(sdr);
		putErrmsg("No space for volatile database.", name);
		return NULL;
	}

	vdb = (IonVdb *) psp(ionwm, vdbAddress);
	memset((char *) vdb, 0, sizeof(IonVdb));
	if ((vdb->nodes = sm_rbt_create(ionwm)) == 0
			|| (vdb->neighbors = sm_rbt_create(ionwm)) == 0
			|| (vdb->contactIndex = sm_rbt_create(ionwm)) == 0
			|| (vdb->rangeIndex = sm_rbt_create(ionwm)) == 0
			|| (vdb->timeline = sm_rbt_create(ionwm)) == 0
			|| (vdb->probes = sm_list_create(ionwm)) == 0
			|| (vdb->requisitions[0] = sm_list_create(ionwm)) == 0
			|| (vdb->requisitions[1] = sm_list_create(ionwm)) == 0
			|| psm_catlg(ionwm, name, vdbAddress) < 0)
	{
		sdr_exit_xn(sdr);
		putErrmsg("Can't initialize volatile database.", name);
		return NULL;
	}

	vdb->clockPid = ERROR;	/*	None yet.		*/
	sdr_read(sdr, (char *) &iondb, getIonDbObject(), sizeof(IonDB));
	vdb->deltaFromUTC = iondb.deltaFromUTC;
	sdr_exit_xn(sdr);	/*	Unlock memory.		*/

	return vdb;
}

