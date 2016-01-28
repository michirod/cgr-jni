/*
 * sdrmgt.c
 *
 *  Created on: 18 nov 2015
 *      Author: michele
 */
#include "sdrmgt.h"

void	sdr_stage(Sdr sdrv, char *into, Object from, long length)
{
	//TODO stub
	if (length == 0)
		return;
	sdr_read(sdrv, into, from, length);
}

Object	Sdr_malloc(const char *file, int line, Sdr sdrv, unsigned long nbytes)
{
	//TODO check sdr type
	return Psm_zalloc(file, line, sdrv, nbytes);
}
void		Sdr_free(const char *file, int line,
				Sdr sdr, Object object)
{
	//TODO check sdr type
	return Psm_free(file, line, sdr, object);
}
