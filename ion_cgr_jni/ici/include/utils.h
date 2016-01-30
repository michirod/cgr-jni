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


PsmPartition getIonPsmPartition(long nodeNum, int partNum);
PsmPartition newIonPsmPartition(long nodeNum, int partNum);
IonDB * createIonDb(Sdr ionsdr, IonDB * iondbPtr);
IonVdb * createIonVdb(char * ionvdbName);

#endif /* JNI_UTILS_H_ */
