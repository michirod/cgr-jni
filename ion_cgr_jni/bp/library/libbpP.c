/*
 * libbpP.c
 *
 *  Created on: 18 nov 2015
 *      Author: michele
 */
#include "bpP.h"
#include "sdrlist.h"
#include "sdrstring.h"
#include "smrbt.h"
#include "ONEtoION_interface.h"


#define NOMINAL_PRIMARY_BLKSIZE	29

void	computePriorClaims(ClProtocol *protocol, Outduct *duct, Bundle *bundle,
		Scalar *priorClaims, Scalar *totalBacklog)
{
	//TODO stub
}

int	guessBundleSize(Bundle *bundle)
{
	return (NOMINAL_PRIMARY_BLKSIZE
		+ bundle->dictionaryLength
		+ bundle->extensionsLength[PRE_PAYLOAD]
		+ bundle->payload.length
		+ bundle->extensionsLength[POST_PAYLOAD]);
}
int	computeECCC(int bundleSize, ClProtocol *protocol)
{
	int	framesNeeded;

	/*	Compute estimated consumption of contact capacity.	*/

	framesNeeded = bundleSize / protocol->payloadBytesPerFrame;
	framesNeeded += (bundleSize % protocol->payloadBytesPerFrame) ? 1 : 0;
	framesNeeded += (framesNeeded == 0) ? 1 : 0;
	return bundleSize + (protocol->overheadPerFrame * framesNeeded);
}

int	orderBpEvents(PsmPartition partition, PsmAddress nodeData,
		void *dataBuffer)
{
	Sdr	sdr = getIonsdr();
	BpEvent	*argEvent;
	Object	elt;
	BpEvent	event;

	if (partition == NULL || nodeData == 0 || dataBuffer == 0)
	{
		putErrmsg("Error calling smrbt BP timeline compare function.",
				NULL);
		return 0;
	}

	argEvent = (BpEvent *) dataBuffer;
	elt = (Object) nodeData;
	sdr_read(sdr, (char *) &event, sdr_list_data(sdr, elt),
			sizeof(BpEvent));
	if (event.time < argEvent->time)
	{
		return -1;
	}

	if (event.time > argEvent->time)
	{
		return 1;
	}

	/*	Same time.						*/

	if (event.ref < argEvent->ref)
	{
		return -1;
	}

	if (event.ref > argEvent->ref)
	{
		return 1;
	}

	/*	Same object.						*/

	if (event.type < argEvent->type)
	{
		return -1;
	}

	if (event.type > argEvent->type)
	{
		return 1;
	}

	return 0;
}

int	bpEnqueue(FwdDirective *directive, Bundle *bundle, Object bundleObj,
		char *proxNodeEid)
{
	Sdr		bpSdr = getIonsdr();
	//PsmPartition	ionwm = getIonwm();
	//BpVdb		*vdb = getBpVdb();
	Object		ductAddr;
	Outduct		duct;
	//PsmAddress	vductElt;
	//VOutduct	*vduct;
	//char		destDuctName[MAX_CL_DUCT_NAME_LEN + 1];
	int		backlogIncrement;
	ClProtocol	protocol;
	time_t		enqueueTime;
	//int		priority;
	//Object		lastElt;

	//CHKERR(ionLocked());
	CHKERR(directive && bundle && bundleObj && proxNodeEid);
	//CHKERR(*proxNodeEid && strlen(proxNodeEid) < MAX_SDRSTRING);
	CHKERR(bundle->ductXmitElt == 0);
	//bpDbTally(BP_DB_FWD_OKAY, bundle->payload.length);

	/*	We have settled on a neighboring node to forward
	 *	this bundle to; if it can't get there because the
	 *	duct to that node is blocked, then the bundle goes
	 *	into limbo until something changes.
	 *
	 *	But if the selected node is the local node (loopback)
	 *	and the bundle has already been delivered, we prevent
	 *	a loopback routing loop by NOT enqueueing the bundle.
	 *	Note that this is a backup check: the scheme-specific
	 *	forwarder should have checked the "delivered" flag
	 *	itself and refrained from trying to enqueue the bundle
	 *	for transmission to the local node.			*/

	if (bundle->delivered)
	{
		/*
		if (isLoopback(proxNodeEid))
		{
			return 0;
		}
		*/
	}

	/*	Next we check to see if the duct is blocked.		*/

	ductAddr = sdr_list_data(bpSdr, directive->outductElt);
	sdr_read(bpSdr, (char *) &duct, ductAddr, sizeof(Outduct));
	if (duct.blocked)
	{
		//return enqueueToLimbo(bundle, bundleObj);
	}

	/*      Now construct transmission parameters.			*/

	bundle->proxNodeEid = sdr_string_create(bpSdr, proxNodeEid);

	bundle->destDuctName = 0;

	sdr_read(bpSdr, (char *) &protocol, duct.protocol, sizeof(ClProtocol));
	backlogIncrement = computeECCC(guessBundleSize(bundle), &protocol);
	if (bundle->enqueueTime == 0)
	{
		bundle->enqueueTime = enqueueTime = getUTCTime();
	}
	else
	{
		enqueueTime = bundle->enqueueTime;
	}

	/*	Insert bundle into the appropriate transmission queue
	 *	of the selected Duct.					*/

	/**
	priority = COS_FLAGS(bundle->bundleProcFlags) & 0x03;
	switch (priority)
	{
	case 0:
		lastElt = sdr_list_last(bpSdr, duct.bulkQueue);
		if (lastElt == 0)
		{
			bundle->ductXmitElt = sdr_list_insert_first(bpSdr,
					duct.bulkQueue, bundleObj);
		}
		else
		{
			bundle->ductXmitElt =
					insertBundleIntoQueue(duct.bulkQueue,
							lastElt, bundleObj, 0, 0, enqueueTime);
		}

		increaseScalar(&duct.bulkBacklog, backlogIncrement);
		break;

	case 1:
		lastElt = sdr_list_last(bpSdr, duct.stdQueue);
		if (lastElt == 0)
		{
			bundle->ductXmitElt = sdr_list_insert_first(bpSdr,
					duct.stdQueue, bundleObj);
		}
		else
		{
			bundle->ductXmitElt =
					insertBundleIntoQueue(duct.stdQueue,
							lastElt, bundleObj, 1, 0, enqueueTime);
		}

		increaseScalar(&duct.stdBacklog, backlogIncrement);
		break;

	default:
		bundle->ductXmitElt = enqueueUrgentBundle(&duct,
				bundle, bundleObj, backlogIncrement);
		increaseScalar(&duct.urgentBacklog, backlogIncrement);
	}
	*/

	sdr_write(bpSdr, ductAddr, (char *) &duct, sizeof(Outduct));
	sdr_write(bpSdr, bundleObj, (char *) bundle, sizeof(Bundle));

	bpEnqueONE(directive, bundle, bundleObj);

	return 0;
}

Object	insertBpTimelineEvent(BpEvent *newEvent)
{
	//TODO stub
	return 1;
}

void	removeBundleFromQueue(Bundle *bundle, Object bundleObj,
		ClProtocol *protocol, Object outductObj, Outduct *outduct)
{
	Sdr		bpSdr = getIonsdr();
	int		backlogDecrement;
	OrdinalState	*ord;

	/*	Removal from queue reduces outduct's backlog.		*/

	backlogDecrement = computeECCC(guessBundleSize(bundle), protocol);
	switch (COS_FLAGS(bundle->bundleProcFlags) & 0x03)
	{
	case 0:				/*	Bulk priority.		*/
		reduceScalar(&(outduct->bulkBacklog), backlogDecrement);
		break;

	case 1:				/*	Standard priority.	*/
		reduceScalar(&(outduct->stdBacklog), backlogDecrement);
		break;

	default:			/*	Urgent priority.	*/
		ord = &(outduct->ordinals[bundle->extendedCOS.ordinal]);
		reduceScalar(&(ord->backlog), backlogDecrement);
		if (ord->lastForOrdinal == bundle->ductXmitElt)
		{
			ord->lastForOrdinal = 0;
		}

		reduceScalar(&(outduct->urgentBacklog), backlogDecrement);
	}

	sdr_write(bpSdr, outductObj, (char *) outduct, sizeof(Outduct));
	sdr_list_delete(bpSdr, bundle->ductXmitElt, NULL, NULL);
	bundle->ductXmitElt = 0;
	sdr_write(bpSdr, bundleObj, (char *) bundle, sizeof(Bundle));
}

int	bpReforwardBundle(Object bundleAddr)
{
	//TODO stub
	return 0;
}

int	bpClone(Bundle *oldBundle, Bundle *newBundle, Object *newBundleObj,
		unsigned int offset, unsigned int length)
{
	//TODO stub
	return 0;
}
