/*
 * ionadmin.h
 *
 *  Created on: 07 dic 2015
 *      Author: michele
 */

#ifndef JNI_INCLUDE_IONADMIN_H_
#define JNI_INCLUDE_IONADMIN_H_


int	runIonadmin(char *cmdFileName);
int	processLine(char *line, int lineLength);
void initializeNode(int tokenCount, char **tokens);

#endif /* JNI_INCLUDE_IONADMIN_H_ */
