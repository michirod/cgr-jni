/*
 *	sdrcatlg.c:	simple data recorder catalogue management
 *			library.
 *
 *	Copyright (c) 2001-2007, California Institute of Technology.
 *	ALL RIGHTS RESERVED.  U.S. Government Sponsorship
 *	acknowledged.
 *
 *	Author: Scott Burleigh, JPL
 *
 *	This library implements the Simple Data Recorder system's
 *	self-delimiting strings.
 *
 *	Modification History:
 *	Date	  Who	What
 *	4-3-96	  APS	Abstracted IPC services and task control.
 *	5-1-96	  APS	Ported to sparc-sunos4.
 *	12-20-00  SCB	Revised for sparc-sunos5.
 *	6-8-07    SCB	Divided sdr.c library into separable components.
 */

#include "psm.h"
#include "sdr.h"
#include "sdrlist.h"

void	Sdr_catlg(const char *file, int line, Sdr sdrv, char *name, int type,
		Object object)
{
	psm_catlg(sdrv, name, object);
}

Object	sdr_find(Sdr sdrv, char *name, int *type)
{
	Object obj = NULL, elt = NULL;
	int result;
	result = psm_locate(sdrv, name, &obj, &elt);
	if (result < 0)
		return 0;
	else if (elt == 0)
		return 0;
	else return obj;
}

void	Sdr_uncatlg(const char *file, int line, Sdr sdrv, char *name)
{
	psm_uncatlg(sdrv, name);
}

