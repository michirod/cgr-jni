/*
 * ionadmin.h
 *
 *  Created on: 07 dic 2015
 *      Author: michele
 */

#ifndef JNI_INCLUDE_IONADMIN_H_
#define JNI_INCLUDE_IONADMIN_H_


int	runIonadmin(const char *cmdFileName);
int	processLine(const char *line, int lineLength);
int runLogadmin(const char *cmdFileName);

#endif /* JNI_INCLUDE_IONADMIN_H_ */
