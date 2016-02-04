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

#define ONEClockClass "core/SimClock"

void init_global();
void finalize_global();
void init_node();
void destroy_node();
long getNodeNum();
void setNodeNum(long nodeNum_new);
time_t getONEReferenceTime();
void setONEReferenceTime(time_t time);
int getTimeFromONE();
time_t getSimulatedUTCTime();
time_t convertIonTimeToOne(time_t ionTime);

#endif /* JNI_INCLUDE_INIT_GLOBAL_H_ */
