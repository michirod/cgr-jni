/*
 * init_global.h
 *
 *  Created on: 23 nov 2015
 *      Author: michele
 */

#ifndef JNI_INCLUDE_INIT_GLOBAL_H_
#define JNI_INCLUDE_INIT_GLOBAL_H_

#include <pthread.h>
#include <time.h>

void init_global();
pthread_key_t get_nodeNum_key();
void init_node(long nodeNum);
long getNodeNum();
void setNodeNum(long nodeNum_new);
time_t getONEReferenceTime();
void setONEReferenceTime(time_t time);

#endif /* JNI_INCLUDE_INIT_GLOBAL_H_ */
