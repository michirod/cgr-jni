/*
 * normal_c.h
 *
 *  Created on: 28 ott 2015
 *      Author: michele
 */

#ifndef JNI_NORMAL_C_H_
#define JNI_NORMAL_C_H_

/**
 * Classico header file, niente da aggiungere
 */

int doSomething(long nodeNum, char * string);
int doSomethingWithLists(long nodeNum, int len, char ** els);
int doSomethingWithPsm(long nodeNum, int len, char ** els);

void *thread_init(void * arg);
void *thread_run(void * arg);
int doSomethingMultithreaded(int len, char ** els);
int doSomethingSimpleWithPsm(int len, char ** els);
#endif /* JNI_NORMAL_C_H_ */
