#include "normal_c.h"

#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "lyst.h"
#include "psm.h"
#include "smlist.h"
#include "utils.h"
#include "init_global.h"
#include "jni_thread.h"

/**
 * Ok, questo e' il codice nativo C: cioe' quello che usa le funzioni delle librerie in C e che non e' da modificare.
 * Nel nostro caso e' praticamente il libcgr.c
 *
 */

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
typedef struct {
	long nodeNum;
	int len;
	char ** args;
} Args;

int doSomething(long nodeNum, char * string)
{
	printf("%s\n", string);
	fflush(stdout);
	return 0;
}

int doSomethingWithLists(long nodeNum, int len, char ** els)
{
	Lyst list = lyst_create_using(42);
	int i;
	for (i = 0; i < len; i++)
	{
		lyst_insert_last(list, els[i]);
	}
	LystElt first = lyst_first(list);
	LystElt last = lyst_last(list);
	LystElt second = lyst_next(first);
	char * firstData = lyst_data(first);
	char * secondData = lyst_data(second);
	char * lastData = lyst_data(last);

	Lyst lyst2 = lyst_create();
	lyst_insert_last(lyst2, firstData);
	if(lyst_first(lyst2) != lyst_last(lyst2))
		printf("ERROR!!! first and last element of the list are not equal\n");
	if(lyst_data(lyst_first(lyst2)) == lyst_data(lyst_last(lyst2)))
			printf("YEAH!!! first and last element of the list are equal\n");

	printf("First: %s, Second: %s, Last: %s\n", firstData, secondData, lastData);
	fflush(stdout);
	return len;
}

int doSomethingWithPsm(long nodeNum, int len, char ** els)
{
	init_node(nodeNum);
	PsmPartition partition = getIonPsmPartition(nodeNum, 1);
	int i;
	char temp[64];
	char * pointer;
	PsmAddress addr, ret, elt;
	for (i = 0; i < len; i++)
	{
		sprintf(temp, "String %d: %s", i, els[i]);
		addr = psm_zalloc(partition, sizeof(char) * strlen(temp) + 1);
		pointer = psp(partition, addr);
		strncpy(pointer, temp, strlen(temp) + 1);
		sprintf(temp, "String%d", i);
		psm_catlg(partition, temp, addr);
	}
	for (i = 0; i < len; i++)
	{
		sprintf(temp, "String%d", i);
		psm_locate(partition, temp, &ret, &elt);
		strcpy(temp, psp(partition, ret));
		printf("%s\n", temp);
		psm_free(partition, ret);
	}
	fflush(stdout);


	PsmAddress list = sm_list_create(partition);
	for (i = 0; i < len; i++)
	{
		sprintf(temp, "String %d: %s", i, els[i]);
		addr = psm_zalloc(partition, sizeof(char) * strlen(temp) + 1);
		pointer = psp(partition, addr);
		strncpy(pointer, temp, strlen(temp) + 1);
		sm_list_insert_last(partition, list, addr);
	}
	sprintf(temp, "vatlatorintalcul");
	addr = psm_zalloc(partition, sizeof(char) * strlen(temp) + 1);
	pointer = psp(partition, addr);
	strncpy(pointer, temp, strlen(temp) + 1);
	PsmAddress first = sm_list_first(partition, list);
	sm_list_insert_after(partition, first, addr);
	PsmAddress last = sm_list_last(partition, list);
	PsmAddress second = sm_list_next(partition, first);
	PsmAddress firstAddress = sm_list_data(partition, first);
	PsmAddress lastAddress = sm_list_data(partition, last);
	PsmAddress secondAddress = sm_list_data(partition, second);
	char * firstData = psp(partition, firstAddress);
	char * secondData = psp(partition, secondAddress);
	char * lastData = psp(partition, lastAddress);

	printf("First: %s, Second: %s, Last: %s\n", firstData, secondData, lastData);
	fflush(stdout);
	return doSomethingMultithreaded(len, els);
}


void *thread_init(void * arg)
{
	long nodeNum = *((long *) arg);
	nodeNum += 10;
	init_node(nodeNum);
	pthread_mutex_unlock(&mutex);
	printf("Node %ld initialized\n", nodeNum);
	return NULL;
}

void *thread_run(void * arg)
{
	Args args = *((Args *) arg);
	long nodeNum = args.nodeNum;
	nodeNum += 10;
	int len = args.len;
	char ** els = args.args;
	setNodeNum(nodeNum);
	doSomethingSimpleWithPsm(len, els);
	pthread_mutex_unlock(&mutex);
	return NULL;
}


int doSomethingMultithreaded(int len, char ** els)
{
	long i;
	pthread_t threads[4];
	Args args;
	args.len = len;
	args.args = els;
	//first do single threaded
	i = 100;
	args.nodeNum = i;
	thread_init(&i);
	thread_run(&args);
	//then do multithreaded
	for (i = 0; i < 4; i++)
	{
		pthread_mutex_lock(&mutex);
		jni_thread_create(&(threads[i]), NULL, thread_init, &i);
	}
	for (i = 0; i < 4; i++)
	{
		pthread_join(threads[i], NULL);
	}
	for (i = 0; i < 4; i++)
	{
		pthread_mutex_lock(&mutex);
		args.nodeNum = i;
		jni_thread_create(&(threads[i]), NULL, thread_run, &args);
	}
	for (i = 0; i < 4; i++)
	{
		pthread_join(threads[i], NULL);
	}
	return 0;
}

int doSomethingSimpleWithPsm(int len, char ** els)
{
	PsmPartition partition = getIonwm();
	int i;
	char temp[64];
	char * pointer;
	PsmAddress addr, ret, elt;
	for (i = 0; i < len; i++)
	{
		sprintf(temp, "String %d: %s", i, els[i]);
		addr = psm_zalloc(partition, sizeof(char) * strlen(temp) + 1);
		pointer = psp(partition, addr);
		strncpy(pointer, temp, strlen(temp) + 1);
		sprintf(temp, "String%d", i);
		psm_catlg(partition, temp, addr);
	}
	for (i = 0; i < len; i++)
	{
		sprintf(temp, "String%d", i);
		psm_locate(partition, temp, &ret, &elt);
		strcpy(temp, psp(partition, ret));
		printf("%s\n", temp);
		psm_free(partition, ret);
	}
	fflush(stdout);


	PsmAddress list = sm_list_create(partition);
	for (i = 0; i < len; i++)
	{
		sprintf(temp, "String %d: %s", i, els[i]);
		addr = psm_zalloc(partition, sizeof(char) * strlen(temp) + 1);
		pointer = psp(partition, addr);
		strncpy(pointer, temp, strlen(temp) + 1);
		sm_list_insert_last(partition, list, addr);
	}
	sprintf(temp, "vatlatorintalcul, nodo %ld", getNodeNum());
	addr = psm_zalloc(partition, sizeof(char) * strlen(temp) + 1);
	pointer = psp(partition, addr);
	strncpy(pointer, temp, strlen(temp) + 1);
	PsmAddress first = sm_list_first(partition, list);
	sm_list_insert_after(partition, first, addr);
	PsmAddress last = sm_list_last(partition, list);
	PsmAddress second = sm_list_next(partition, first);
	PsmAddress firstAddress = sm_list_data(partition, first);
	PsmAddress lastAddress = sm_list_data(partition, last);
	PsmAddress secondAddress = sm_list_data(partition, second);
	char * firstData = psp(partition, firstAddress);
	char * secondData = psp(partition, secondAddress);
	char * lastData = psp(partition, lastAddress);

	printf("NODE %ld: First: %s, Second: %s, Last: %s\n", getNodeNum(), firstData, secondData, lastData);
	fflush(stdout);
	return len;
}
