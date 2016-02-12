/*
 * platform_sm.c
 *
 *  Created on: 18 nov 2015
 *      Author: michele
 */
#include "platform.h"

sm_SemId		sm_SemCreate(int key, int semType)
{
	//TODO stub
	return 0;
}

void		sm_SemDelete(sm_SemId semId)
{
	//TODO stub
}

int		sm_SemUnwedge(sm_SemId semId, int timeoutSeconds)
{
	//TODO stub
	return 0;
}

int		sm_SemTake(sm_SemId semId)
{
	//TODO stub
	return 1;
}
void		sm_SemGive(sm_SemId semId)
{
	//TODO stub
}
void		sm_SemEnd(sm_SemId semId)
{
	//TODO stub
}
void		sm_TaskKill(int taskId, int sigNbr)
{
	//TODO stub
}
int		sm_TaskExists(int taskId)
{
	//TODO stub
	return 0;
}
