/*
 * ion.c
 *
 *  Created on: 18 nov 2015
 *      Author: michele
 */

#include <pthread.h>

#include "ion.h"
#include "rfx.h"
#include "platform.h"
#include "shared.h"
#include "utils.h"
#include "init_global.h"

#ifndef NODE_LIST_SEMKEY
#define NODE_LIST_SEMKEY	(0xeeee1)
#endif

#define	ION_DEFAULT_SM_KEY	((255 * 256) + 1)
#define	ION_SM_NAME		"ionwm"
#define	ION_DEFAULT_SDR_NAME	"ion"

#define timestampInFormat	"%4d/%2d/%2d-%2d:%2d:%2d"
#define timestampOutFormat	"%.4d/%.2d/%.2d-%.2d:%.2d:%.2d"

static char	*_iondbName()
{
	return "iondb";
}

static char	*_ionvdbName()
{
	return "ionvdb";
}

char * getIonvdbName()
{
	return _ionvdbName();
}

IonVdb	*getIonVdb()
{
	IonVdb	*vdb = NULL;
	PsmAddress	vdbAddress;
	PsmAddress	elt;
	PsmPartition	ionwm;
	char * name = _ionvdbName();

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
		return vdb;
	}

	return NULL;
}

PsmPartition getIonwm()
{
	return getIonPsmPartition(getNodeNum(), WM_PSM_PARTITION);
}
static void destroyIonwm()
{
	eraseIonPsmPartition(getNodeNum(), WM_PSM_PARTITION);
}

Sdr	getIonsdr()
{
	return getIonPsmPartition(getNodeNum(), SDR_PSM_PARTITION);
}
static void destroyIonsdr()
{
	eraseIonPsmPartition(getNodeNum(), SDR_PSM_PARTITION);
}

uvast	getOwnNodeNbr()
{
	return getNodeNum();
}

int	getIonMemoryMgr()
{
	//TODO stub
	return 0;
}

Object	getIonDbObject()
{
	//TODO stub
	Object result;
	result = sdr_find(getIonsdr(), _iondbName(), NULL);
	return result;
	//return _iondbObject(NULL);
}

void	*allocFromIonMemory(const char *fileName, int lineNbr, size_t length)
{
	return (void*) malloc(length);
}

void	releaseToIonMemory(const char *fileName, int lineNbr, void *block)
{
	free(block);
}

static time_t	readTimestamp(char *timestampBuffer, time_t referenceTime,
			int timestampIsUTC)
{
	long		interval = 0;
	struct tm	ts;
	int		count;

	if (timestampBuffer == NULL)
	{
		return 0;
	}

	if (*timestampBuffer == '+')	/*	Relative time.		*/
	{
		interval = strtol(timestampBuffer + 1, NULL, 0);
		return referenceTime + interval;
	}

	memset((char *) &ts, 0, sizeof ts);
	count = sscanf(timestampBuffer, timestampInFormat, &ts.tm_year,
		&ts.tm_mon, &ts.tm_mday, &ts.tm_hour, &ts.tm_min, &ts.tm_sec);
	if (count != 6)
	{
		return 0;
	}

	ts.tm_year -= 1900;
	ts.tm_mon -= 1;
	ts.tm_isdst = 0;		/*	Default is UTC.		*/
#ifndef VXWORKS
#ifdef mingw
	_tzset();	/*	Need to orient mktime properly.		*/
#else
	tzset();	/*	Need to orient mktime properly.		*/
#endif
	if (timestampIsUTC)
	{
		/*	Must convert UTC time to local time for mktime.	*/

#if defined (freebsd)
		ts.tm_sec -= ts.tm_gmtoff;
#elif defined (RTEMS)
		/*	RTEMS has no concept of time zones.		*/
#elif defined (mingw)
		ts.tm_sec -= _timezone;
#else
		ts.tm_sec -= timezone;
#endif
	}
	else	/*	Local time already; may or may not be DST.	*/
	{
		ts.tm_isdst = -1;
	}
#endif
	return mktime(&ts);
}

time_t	readTimestampUTC(char *timestampBuffer, time_t referenceTime)
{
	return readTimestamp(timestampBuffer, referenceTime, 1);
}

void	writeTimestampUTC(time_t timestamp, char *timestampBuffer)
{
	struct tm	ts;

	CHKVOID(timestampBuffer);
	oK(gmtime_r(&timestamp, &ts));
	snprintf(timestampBuffer, 20, timestampOutFormat,
			ts.tm_year + 1900, ts.tm_mon + 1, ts.tm_mday,
			ts.tm_hour, ts.tm_min, ts.tm_sec);
}

time_t	getUTCTime()
{
	//TODO stub
	// this must get the current time from ONE
	return getSimulatedUTCTime();
}

static int	checkNodeListParms(IonParms *parms, char *wdName, uvast nodeNbr)
{
	char		*nodeListDir;
	sm_SemId	nodeListMutex;
	char		nodeListFileName[265];
	int		nodeListFile;
	int		lineNbr = 0;
	int		lineLen;
	char		lineBuf[256];
	uvast		lineNodeNbr;
	int		lineWmKey;
	char		lineSdrName[MAX_SDR_NAME + 1];
	char		lineWdName[256];
	int		result;

	nodeListDir = getenv("ION_NODE_LIST_DIR");
	if (nodeListDir == NULL)	/*	Single node on machine.	*/
	{
		if (parms->wmKey == 0)
		{
			parms->wmKey = ION_DEFAULT_SM_KEY;
		}

		if (parms->wmKey != ION_DEFAULT_SM_KEY)
		{
			putErrmsg("Config parms wmKey != default.",
					itoa(ION_DEFAULT_SM_KEY));
			return -1;
		}

		if (parms->sdrName[0] == '\0')
		{
			istrcpy(parms->sdrName, ION_DEFAULT_SDR_NAME,
					sizeof parms->sdrName);
		}

		if (strcmp(parms->sdrName, ION_DEFAULT_SDR_NAME) != 0)
		{
			putErrmsg("Config parms sdrName != default.",
					ION_DEFAULT_SDR_NAME);
			return -1;
		}

		return 0;
	}

	/*	Configured for multi-node operation.			*/

	nodeListMutex = sm_SemCreate(NODE_LIST_SEMKEY, SM_SEM_FIFO);
	if (nodeListMutex == SM_SEM_NONE
	|| sm_SemUnwedge(nodeListMutex, 3) < 0 || sm_SemTake(nodeListMutex) < 0)
	{
		putErrmsg("Can't lock node list file.", NULL);
		return -1;
	}

	snprintf(nodeListFileName, sizeof nodeListFileName, "%.255s%cion_nodes",
			nodeListDir, ION_PATH_DELIMITER);
	if (nodeNbr == 0)	/*	Just attaching.			*/
	{
		nodeListFile = iopen(nodeListFileName, O_RDONLY, 0);
	}
	else			/*	Initializing the node.		*/
	{
		nodeListFile = iopen(nodeListFileName, O_RDWR | O_CREAT, 0666);
	}

	if (nodeListFile < 0)
	{
		sm_SemGive(nodeListMutex);
		putSysErrmsg("Can't open ion_nodes file", nodeListFileName);
		//writeMemo("[?] Remove ION_NODE_LIST_DIR from env?");
		return -1;
	}

	while (1)
	{
		if (igets(nodeListFile, lineBuf, sizeof lineBuf, &lineLen)
				== NULL)
		{
			if (lineLen < 0)
			{
				close(nodeListFile);
				sm_SemGive(nodeListMutex);
				putErrmsg("Failed reading ion_nodes file.",
						nodeListFileName);
				return -1;
			}

			break;		/*	End of file.		*/
		}

		lineNbr++;
		if (sscanf(lineBuf, UVAST_FIELDSPEC " %d %31s %255s",
			&lineNodeNbr, &lineWmKey, lineSdrName, lineWdName) < 4)
		{
			close(nodeListFile);
			sm_SemGive(nodeListMutex);
			putErrmsg("Syntax error at line#", itoa(lineNbr));
			writeMemoNote("[?] Repair ion_nodes file.",
					nodeListFileName);
			return -1;
		}

		if (lineNodeNbr == nodeNbr)		/*	Match.	*/
		{
			/*	lineNodeNbr can't be zero (we never
			 *	write such lines to the file), so this
			 *	must be matching non-zero node numbers.
			 *	So we are re-initializing this node.	*/

			close(nodeListFile);
			if (strcmp(lineWdName, wdName) != 0)
			{
				sm_SemGive(nodeListMutex);
				putErrmsg("CWD conflict at line#",
						itoa(lineNbr));
				writeMemoNote("[?] Repair ion_nodes file.",
						nodeListFileName);
				return -1;
			}

			if (parms->wmKey == 0)
			{
				parms->wmKey = lineWmKey;
			}

			if (parms->wmKey != lineWmKey)
			{
				sm_SemGive(nodeListMutex);
				putErrmsg("WmKey conflict at line#",
						itoa(lineNbr));
				writeMemoNote("[?] Repair ion_nodes file.",
						nodeListFileName);
				return -1;
			}

			if (parms->sdrName[0] == '\0')
			{
				istrcpy(parms->sdrName, lineSdrName,
						sizeof parms->sdrName);
			}

			if (strcmp(parms->sdrName, lineSdrName) != 0)
			{
				sm_SemGive(nodeListMutex);
				putErrmsg("SdrName conflict at line#",
						itoa(lineNbr));
				writeMemoNote("[?] Repair ion_nodes file.",
						nodeListFileName);
				return -1;
			}

			return 0;
		}

		/*	lineNodeNbr does not match nodeNbr (which may
		 *	be zero).					*/

		if (strcmp(lineWdName, wdName) == 0)	/*	Match.	*/
		{
			close(nodeListFile);
			sm_SemGive(nodeListMutex);
			if (nodeNbr == 0)	/*	Attaching.	*/
			{
				parms->wmKey = lineWmKey;
				istrcpy(parms->sdrName, lineSdrName,
						MAX_SDR_NAME + 1);
				return 0;
			}

			/*	Reinitialization conflict.		*/

			putErrmsg("NodeNbr conflict at line#", itoa(lineNbr));
			writeMemoNote("[?] Repair ion_nodes file.",
					nodeListFileName);
			return -1;
		}

		/*	Haven't found matching line yet.  Continue.	*/
	}

	/*	No matching lines in file.				*/

	if (nodeNbr == 0)	/*	Attaching to existing node.	*/
	{
		close(nodeListFile);
		sm_SemGive(nodeListMutex);
		putErrmsg("No node has been initialized in this directory.",
				wdName);
		return -1;
	}

	/*	Initializing, so append line to the nodes list file.	*/

	if (parms->wmKey == 0)
	{
		parms->wmKey = ION_DEFAULT_SM_KEY;
	}

	if (parms->sdrName[0] == '\0')
	{
		istrcpy(parms->sdrName, ION_DEFAULT_SDR_NAME,
				sizeof parms->sdrName);
	}

	snprintf(lineBuf, sizeof lineBuf, UVAST_FIELDSPEC " %d %.31s %.255s\n",
			nodeNbr, parms->wmKey, parms->sdrName, wdName);
	result = iputs(nodeListFile, lineBuf);
	close(nodeListFile);
	sm_SemGive(nodeListMutex);
	if (result < 0)
	{
		putErrmsg("Failed writing to ion_nodes file.", NULL);
		return -1;
	}

	return 0;
}

int	ionInitialize(IonParms *parms, uvast ownNodeNbr)
{
	Sdr		ionsdr;
	Object		iondbObject;
	IonDB		iondbBuf;
	char		*ionvdbName = _ionvdbName();
	//ZcoCallback	notify = ionProvideZcoSpace;

	//CHKERR(parms);
	CHKERR(ownNodeNbr);
#ifdef mingw
	if (_winsock(0) < 0)
	{
		return -1;
	}
#endif
	/*
	if (igetcwd(wdname, 256) == NULL)
	{
		putErrmsg("Can't get cwd name.", NULL);
		return -1;
	}

	if (parms->sdrWmSize <= 0)
	{
		parms->sdrWmSize = 1000000;	//	Default.
	}

	if (checkNodeListParms(parms, wdname, ownNodeNbr) < 0)
	{
		putErrmsg("Failed checking node list parms.", NULL);
		return -1;
	}

	if (sdr_initialize(parms->sdrWmSize, NULL, SM_NO_KEY, NULL) < 0)
	{
		putErrmsg("Can't initialize the SDR system.", NULL);
		return -1;
	}

	if (sdr_load_profile(parms->sdrName, parms->configFlags,
			parms->heapWords, parms->heapKey, parms->logSize,
			parms->logKey, parms->pathName, "ionrestart") < 0)
	{
		putErrmsg("Unable to load SDR profile for ION.", NULL);
		return -1;
	}

	ionsdr = sdr_start_using(parms->sdrName);
	if (ionsdr == NULL)
	{
		putErrmsg("Can't start using SDR for ION.", NULL);
		return -1;
	}
	*/

	ionsdr = getIonsdr();

	/*	Recover the ION database, creating it if necessary.	*/

	CHKERR(sdr_begin_xn(ionsdr));
	iondbObject = sdr_find(ionsdr, _iondbName(), NULL);
	if (iondbObject < 0)
	{	/*	SDR error.			*/
		sdr_cancel_xn(ionsdr);
		putErrmsg("Can't seek ION database in SDR.", NULL);
		return -1;
	}
	else if (iondbObject == 0)
	{   /*	Not found; must create new DB.	*/
		if (ownNodeNbr == 0)
		{
			sdr_cancel_xn(ionsdr);
			putErrmsg("Must supply non-zero node number.", NULL);
			return -1;
		}

		createIonDb(ionsdr, &iondbBuf);
		iondbObject = sdr_malloc(ionsdr, sizeof(IonDB));
		if (iondbObject == 0)
		{
			sdr_cancel_xn(ionsdr);
			putErrmsg("No space for database.", NULL);
			return -1;
		}

		sdr_write(ionsdr, iondbObject, (char *) &iondbBuf,
				sizeof(IonDB));
		sdr_catlg(ionsdr, _iondbName(), 0, iondbObject);
		if (sdr_end_xn(ionsdr))
		{
			putErrmsg("Can't create ION database.", NULL);
			return -1;
		}
	}

	else		/*	Found DB in the SDR.		*/
		sdr_exit_xn(ionsdr);

	/*	Open ION shared-memory partition.			*/

	if (getIonwm() == NULL)
	{
		putErrmsg("ION memory configuration failed.", NULL);
		return -1;
	}

	if (createIonVdb(ionvdbName) == NULL)
	{
		putErrmsg("ION can't initialize vdb.", NULL);
		return -1;
	}
	return 0;
}

void	ionTerminate()
{
	destroyIonVdb(_ionvdbName());
	destroyIonDb(_iondbName());
	destroyIonsdr();
	destroyIonwm();
}
