/*
 * chsim.c
 *
 *  Created on: 08 feb 2016
 *      Author: michele
 */

#include <sdr.h>
#include <rfx.h>
#include <utils.h>
#include <lyst.h>

#include <init_global.h>

static void	freeLystContents(LystElt elt, void *userdata)
{
	free(lyst_data(elt));
}

static int findDiscoveredContacts(uvast localNodeNbr, Lyst list)
{
	uvast currentNodeNum = getNodeNum();
	setNodeNum(localNodeNbr);
	Sdr sdr = getIonsdr();
	Object iondbObj = getIonDbObject();
	IonDB iondb;
	IonContact buf;
	IonContact * contact;
	Object elt = NULL;
	int count = 0;
	sdr_read(sdr, (char*) &iondb, iondbObj, sizeof(IonDB));
	elt = sdr_list_first(sdr, iondb.contacts);
	while (elt != NULL)
	{
		sdr_read(sdr, (char*) &buf, sdr_list_data(sdr, elt), sizeof(IonContact));
		if (buf.discovered == 1)
		{
			contact = malloc(sizeof(IonContact));
			memcpy(contact, &buf, sizeof(IonContact));
			count++;
			lyst_insert_last(list, contact);
		}
		elt = sdr_list_next(sdr, elt);
	}
	setNodeNum(currentNodeNum);
	return count;
}

static void copyDiscoveredContacts(Lyst from, int toNode)
{
	uvast currentNodeNum = getNodeNum();
	time_t currentTime = getUTCTime();
	IonContact * contact;
	setNodeNum(toNode);
	LystElt elt = lyst_first(from);
	uvast first, last;
	PsmAddress xaddr;
char buf1[255], buf2[255];
	while (elt != NULL)
	{
		contact = lyst_data(elt);
		/*
		 *  Range is bidirectional, so I need to put the least
		 *  node number as fromNode.
		 */
		if (contact->fromNode < contact->toNode)
		{
			first = contact->fromNode;
			last = contact->toNode;
		}
		else
		{
			first = contact->toNode;
			last = contact->fromNode;
		}
		rfx_insert_range(currentTime, MAX_POSIX_TIME, first, last,
				0, &xaddr);
		rfx_insert_contact(contact->fromTime, contact->toTime, first,
				last, contact->xmitRate, contact->confidence, &xaddr);
		rfx_insert_contact(contact->fromTime, contact->toTime, last,
				first, contact->xmitRate, contact->confidence, &xaddr);
		elt = lyst_next(elt);
writeTimestampLocal(contact->fromTime, buf1);
writeTimestampLocal(contact->toTime, buf2);
printf ("Inserting range and symmetric contacts, from %d to %d"
		"start %s end %s.\n", first, last, buf1, buf2);
	}
	lyst_clear(from);
	setNodeNum(currentNodeNum);
}

void exchangeCurrentDiscoveredContacts(uvast node1, uvast node2)
{
	Lyst discoveredContacts[2];
	int found[2];
	discoveredContacts[0] = lyst_create();
	discoveredContacts[1] = lyst_create();
	found[0] = findDiscoveredContacts(node1, discoveredContacts[0]);
	found[1] = findDiscoveredContacts(node2, discoveredContacts[1]);
	// copy current discovered contact from node1 to node2
	printf("COPYING CURRENT DISCOVERED CONTACTS FROM NODE %d TO NODE %d\n", node1, node2);
	copyDiscoveredContacts(discoveredContacts[0], node2);
	// copy current discovered contact from node2 to node1
	printf("COPYING CURRENT DISCOVERED CONTACTS FROM NODE %d TO NODE %d\n", node2, node1);
	copyDiscoveredContacts(discoveredContacts[1], node1);
	lyst_destroy(discoveredContacts[0]);
	lyst_destroy(discoveredContacts[1]);
	fflush(stdout);
}

void newContactDiscovered(uvast neighborNode, unsigned int xmitRate)
{
	uvast first, last;
	first = getNodeNum();
	time_t currentTime = getUTCTime();
	PsmAddress xaddr;
	/*
	 *  Range is bidirectional, so I need to put the least
	 *  node number as fromNode.
	 */
	if (first > neighborNode)
	{
		last = first;
		first = neighborNode;
	}
	else
		last = neighborNode;
	rfx_insert_range(currentTime, MAX_POSIX_TIME, first,
			last, 0, &xaddr);
	rfx_insert_contact(currentTime, 0, first, last,
			xmitRate, 1, &xaddr);
	rfx_insert_contact(currentTime, 0, last, first,
				xmitRate, 1, &xaddr);
	fflush(stdout);
}

void contactLost(uvast neighborNode)
{
	rfx_remove_discovered_contacts(neighborNode);
	fflush(stdout);
}
static void getContactLog(uvast localNodeNbr, int idx, Lyst log)
{
	uvast currentNodeNum = getNodeNum();
	setNodeNum(localNodeNbr);
	Sdr sdr = getIonsdr();
	Object iondbObj = getIonDbObject();
	IonDB iondb;
	PastContact * contact;
	Object elt;
	sdr_read(sdr, (char*) &iondb, iondbObj, sizeof(IonDB));
	elt = sdr_list_first(sdr, iondb.contactLog[idx]);
	while (elt != NULL)
	{
		contact = malloc(sizeof(PastContact));
		sdr_read(sdr, (char*) contact, sdr_list_data(sdr, elt), sizeof(PastContact));
		lyst_insert_last(log, contact);
		elt = sdr_list_next(sdr, elt);
	}
	setNodeNum(currentNodeNum);
}

static void copyContactHistory(Lyst from, uvast toNode, int idx)
{
	uvast currentNodeNum = getNodeNum();
	PastContact * contact;
	setNodeNum(toNode);
	LystElt elt = lyst_first(from);
	while (elt != NULL)
	{
		contact = lyst_data(elt);
		rfx_log_discovered_contact(contact->fromTime, contact->toTime,
				contact->fromNode, contact->toNode, contact->xmitRate,
				idx);
		elt = lyst_next(elt);
	}
	lyst_clear(from);
	setNodeNum(currentNodeNum);
}

void exchangeContactHistory(uvast node1, uvast node2)
{
	Lyst node1Log[2], node2Log[2];
	node1Log[0] = lyst_create();
	node1Log[1] = lyst_create();
	node2Log[0] = lyst_create();
	node2Log[1] = lyst_create();

	lyst_delete_set(node1Log[0], freeLystContents, NULL);
	lyst_delete_set(node1Log[1], freeLystContents, NULL);
	lyst_delete_set(node2Log[0], freeLystContents, NULL);
	lyst_delete_set(node2Log[1], freeLystContents, NULL);

	getContactLog(node1, SENDER_NODE, node1Log[0]);
	getContactLog(node1, RECEIVER_NODE, node1Log[1]);
	getContactLog(node2, SENDER_NODE, node2Log[0]);
	getContactLog(node2, RECEIVER_NODE, node2Log[1]);

	printf("COPYING CONTACT HISTORY FROM NODE %d TO NODE %d\n", node1, node2);
	copyContactHistory(node1Log[0], node2, SENDER_NODE);
	copyContactHistory(node1Log[1], node2, RECEIVER_NODE);
	printf("COPYING CONTACT HISTORY FROM NODE %d TO NODE %d\n", node2, node1);
	copyContactHistory(node2Log[0], node1, SENDER_NODE);
	copyContactHistory(node2Log[1], node1, RECEIVER_NODE);
	fflush(stdout);

	lyst_destroy(node1Log[0]);
	lyst_destroy(node1Log[1]);
	lyst_destroy(node2Log[0]);
	lyst_destroy(node2Log[1]);
}

void predictContacts()
{
	rfx_predict_all_contacts();
	fflush(stdout);
}
