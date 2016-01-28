/*
 *	sdrstring.c:	simple data recorder string management
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

#include "sdrstring.h"

/*		Private definition of SDR string structure.		*/

typedef unsigned char	SdrStringBuffer[SDRSTRING_BUFSZ];
			/*	SdrString is a low-overhead string
				representation specially designed for
				efficient storage of small strings in
				the SDR.  To avoid time-consuming SDR
				I/O to find the end of a string when
				retrieving it from the SDR, we do not
				NULL-terminate SdrStrings.  Instead
				the first character contains the length
				of the string, which cannot exceed 255.	*/

/*	*	*	String management functions	*	*	*/

Object	Sdr_string_create(const char *file, int line, Sdr sdrv, char *from)
{
	long		length = 0;
	Object		string;
	SdrStringBuffer stringBuf;

	if (from == NULL || (length = strlen(from)) > 255)
	{
		return 0;
	}

	string = sdr_malloc(sdrv, length + 1);
	if (string == 0)
	{
		return 0;
	}

	stringBuf[0] = length;
	memcpy(stringBuf + 1, from, length);
	sdr_write(sdrv, string, stringBuf, length + 1);
	return string;
}

int	sdr_string_length(Sdr sdrv, Object string)
{
	unsigned char	length;

	CHKERR(string);
	sdr_read(sdrv, length, string, 1);

	return length;
}

int	sdr_string_read(Sdr sdrv, char *into, Object string)
{
	Address		addr = (Address) string;
	unsigned char	length;
	CHKERR(into);
	CHKERR(string);
	length = (unsigned char) sdr_string_length(sdrv, string);
	sdr_read(sdrv, into, addr + 1, length);
	into[length] = '\0';
	return length;
}
