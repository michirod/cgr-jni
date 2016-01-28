/*
 * sdrlist.c
#include "sdrP.h"
 *
 *  Created on: 18 nov 2015
 *      Author: michele
 */

#include "sdrlist.h"

#include "smlist.h"

/*		Private definitions of SDR list structures.		*/

typedef struct
{
	Address		userData;
	Object		first;	/*	first element in the list	*/
	Object		last;	/*	last element in the list	*/
	unsigned long   length;	/*	number of elements in the list	*/
} SdrList;

typedef struct
{
	Object		list;	/*	list that this element is in	*/
	Object		prev;	/*	previous element in list	*/
	Object		next;	/*	next element in list		*/
	Object		data;	/*	data for this element		*/
} SdrListElt;

/*	*	*	List management functions	*	*	*/



Address	sdr_list_data(Sdr sdrv, Object elt)
{
	//TODO stub
	//SdrListElt	eltBuffer;
	//CHKZERO(elt);
	//sdrFetch(eltBuffer, (Address) elt);
	//return eltBuffer.data;
	return sm_list_data(sdrv, elt);
}

Object	sdr_list_next(Sdr sdrv, Object elt)
{
	//TODO stub
	//SdrListElt	eltBuffer;

	//CHKZERO(sdrFetchSafe(sdrv));
	CHKZERO(elt);
	//sdrFetch(eltBuffer, (Address) elt);
	//return eltBuffer.next;
	return sm_list_next(sdrv, elt);
}

Object	sdr_list_prev(Sdr sdrv, Object elt)
{
	//TODO stub
	//SdrListElt	eltBuffer;

	//CHKZERO(sdrFetchSafe(sdrv));
	CHKZERO(elt);
	//sdrFetch(eltBuffer, (Address) elt);
	//return eltBuffer.prev;
	return sm_list_prev(sdrv, elt);
}
Object	sdr_list_first(Sdr sdrv, Object list)
{
	//TODO stub
	//SdrList		listBuffer;
	//CHKZERO(sdrFetchSafe(sdrv));
	CHKZERO(list);
	//sdrFetch(listBuffer, (Address) list);
	//return listBuffer.first;
	return sm_list_first(sdrv, list);
}

Object	sdr_list_last(Sdr sdrv, Object list)
{
	//TODO stub
	//SdrList		listBuffer;

	//CHKZERO(sdrFetchSafe(sdrv));
	CHKZERO(list);
	//sdrFetch(listBuffer, (Address) list);
	//return listBuffer.last;
	return sm_list_last(sdrv, list);
}

void	Sdr_list_delete(const char *file, int line, Sdr sdrv, Object elt,
		SdrListDeleteFn deleteFn, void *arg)
{
	Sm_list_delete(file, line, sdrv, elt, deleteFn, arg);
}

Object		Sdr_list_insert_last(const char *file, int line,
		Sdr sdr, Object list, Address data)
{
	return Sm_list_insert_last(file, line, sdr, list, data);
}
