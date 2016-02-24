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

#define WM_PSM_PARTITION 0
#define SDR_PSM_PARTITION 1


void destroyIonPartitions();
PsmPartition getIonPsmPartition(long nodeNum, int partNum);
PsmPartition newIonPsmPartition(long nodeNum, int partNum);
void eraseIonPsmPartition(long nodeNum, int partNum);
void initIonWm();
void destroyIonWm();
PsmPartition getIonWm();
void initIonSdr();
void destroyIonSdr();
Sdr getIonSdr();
IonDB * createIonDb(Sdr ionsdr, IonDB * iondbPtr);
void destroyIonDb(char * iondbName);
IonVdb * createIonVdb(char * ionvdbName);
void destroyIonVdb(char * ionvdbName);

#endif /* JNI_UTILS_H_ */
