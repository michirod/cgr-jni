/*
 * utils.h
 *
 *  Created on: 04 nov 2015
 *      Author: michele
 */

#ifndef JNI_UTILS_H_
#define JNI_UTILS_H_

#include "ion.h"
#include "psm.h"
#include "sdr.h"

#define ONEClockClass "core/SimClock"

PsmPartition getIonPsmPartition(long nodeNum, int partNum);
PsmPartition newIonPsmPartition(long nodeNum, int partNum);
IonDB * createIonDb(Sdr ionsdr, IonDB * iondbPtr);
IonVdb * createIonVdb(char * ionvdbName);
int getTimeFromONE();
time_t getSimulatedUTCTime();

#endif /* JNI_UTILS_H_ */
