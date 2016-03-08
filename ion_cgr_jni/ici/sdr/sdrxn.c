/*
 * sdrxn.c
 *
 *  Created on: 18 nov 2015
 *      Author: michele
 */
#include <stdlib.h>

#include "sdr.h"

int	sdr_begin_xn(Sdr sdrv)
{
	// NO OP
	return 1;
}
void	sdr_exit_xn(Sdr sdrv)
{
	// NO OP
}
void		sdr_cancel_xn(Sdr sdr)
{
	// NO OP
}
int	sdr_end_xn(Sdr sdrv)
{
	// NO OP
	return 0;
}
void	sdr_read(Sdr sdrv, char *into, Address from, long length)
{
	// NO OP
	//_sdrfetch(sdrv, into, from, length);
	if (from == NULL)
	{
		putErrmsg("Error null pointer", "from");
		return;
	}
	if (into == NULL)
	{
		putErrmsg("Error null pointer", "into");
		return;
	}
	void * ptr = psp(sdrv, from);
	memcpy(into, ptr, length);
}
void	Sdr_write(const char *file, int line, Sdr sdrv, Address into,
		char *from, long length)
{
	// NO OP
	void * ptr = psp(sdrv, into);
	memcpy(ptr, from, length);
}
