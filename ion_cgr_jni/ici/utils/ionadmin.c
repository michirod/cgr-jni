/*
 * ionadmin.c
 *
 *  Created on: 01 dic 2015
 *      Author: michele
 */

#include <stdlib.h>

#include "ion.h"
#include "platform.h"
#include "rfx.h"

#define SYNTAX_ERROR printf("%s\n", "Syntax error.")

static void	printText(char *text)
{
	/**
	if (_echo(NULL))
	{
		writeMemo(text);
	}
	**/
	PUTS(text);
}

static time_t	_referenceTime(time_t *newValue)
{
	static time_t	reftime = 0;

	if (newValue)
	{
		reftime = *newValue;
	}

	return reftime;
}

void executeAdd(int tokenCount, char **tokens)
{
	time_t		refTime;
	time_t		fromTime;
	time_t		toTime;
	uvast		fromNodeNbr;
	uvast		toNodeNbr;
	unsigned int	xmitRate;
	float		prob;
	unsigned int	owlt;

	if (tokenCount < 2)
	{
		//printText("Add what?");
		return;
	}

	switch (tokenCount)
	{
	case 8:
		prob = atof(tokens[7]);
		break;

	case 7:
		prob = 1.0;
		break;

	default:
		SYNTAX_ERROR;
		return;
	}

	refTime = _referenceTime(NULL);
	fromTime = readTimestampUTC(tokens[2], refTime);
	toTime = readTimestampUTC(tokens[3], refTime);
	if (toTime <= fromTime)
	{
		printText("Interval end time must be later than start time.");
		return;
	}

	fromNodeNbr = strtouvast(tokens[4]);
	toNodeNbr = strtouvast(tokens[5]);
	if (strcmp(tokens[1], "contact") == 0)
	{
		xmitRate = strtol(tokens[6], NULL, 0);
		oK(rfx_insert_contact(fromTime, toTime, fromNodeNbr,
				toNodeNbr, xmitRate, prob));
		//oK(_forecastNeeded(1));
		return;
	}

	if (strcmp(tokens[1], "range") == 0)
	{
		owlt = atoi(tokens[6]);
		oK(rfx_insert_range(fromTime, toTime, fromNodeNbr,
				toNodeNbr, owlt));
		return;
	}

	SYNTAX_ERROR;
}
static void	executeDelete(int tokenCount, char **tokens)
{
	time_t	refTime;
	time_t	timestamp;
	uvast	fromNodeNbr;
	uvast	toNodeNbr;

	if (tokenCount < 2)
	{
		printText("Delete what?");
		return;
	}

	if (tokenCount != 5)
	{
		SYNTAX_ERROR;
		return;
	}

	if (tokens[2][0] == '*')
	{
		timestamp = 0;
	}
	else
	{
		refTime = _referenceTime(NULL);
		timestamp = readTimestampUTC(tokens[2], refTime);
		if (timestamp == 0)
		{
			SYNTAX_ERROR;
			return;
		}
	}

	fromNodeNbr = strtouvast(tokens[3]);
	toNodeNbr = strtouvast(tokens[4]);
	if (strcmp(tokens[1], "contact") == 0)
	{
		oK(rfx_remove_contact(timestamp, fromNodeNbr, toNodeNbr));
		//oK(_forecastNeeded(1));
		return;
	}

	if (strcmp(tokens[1], "range") == 0)
	{
		oK(rfx_remove_range(timestamp, fromNodeNbr, toNodeNbr));
		return;
	}

	SYNTAX_ERROR;
}

static void	executeInfo(int tokenCount, char **tokens)
{
	Sdr		sdr = getIonsdr();
	PsmPartition	ionwm = getIonwm();
	IonVdb		*vdb = getIonVdb();
	time_t		refTime;
	time_t		timestamp;
	uvast		fromNode;
	uvast		toNode;
	IonCXref	arg1;
	PsmAddress	elt;
	PsmAddress	addr;
	PsmAddress	nextElt;
	char		buffer[RFX_NOTE_LEN];
	IonRXref	arg2;

	if (tokenCount < 2)
	{
		printText("Information on what?");
		return;
	}

	if (tokenCount != 5)
	{
		SYNTAX_ERROR;
		return;
	}

	refTime = _referenceTime(NULL);
	timestamp = readTimestampUTC(tokens[2], refTime);
	fromNode = strtouvast(tokens[3]);
	toNode = strtouvast(tokens[4]);
	if (strcmp(tokens[1], "contact") == 0)
	{
		memset((char *) &arg1, 0, sizeof(IonCXref));
		arg1.fromNode = fromNode;
		arg1.toNode = toNode;
		arg1.fromTime = timestamp;
		CHKVOID(sdr_begin_xn(sdr));
		elt = sm_rbt_search(ionwm, vdb->contactIndex,
				rfx_order_contacts, &arg1, &nextElt);
		if (elt)
		{
			addr = sm_rbt_data(ionwm, elt);
			oK(rfx_print_contact(addr, buffer));
			printText(buffer);
		}
		else
		{
			printText("Contact not found in database.");
		}

		sdr_exit_xn(sdr);
		return;
	}

	if (strcmp(tokens[1], "range") == 0)
	{
		memset((char *) &arg2, 0, sizeof(IonRXref));
		arg2.fromNode = fromNode;
		arg2.toNode = toNode;
		arg2.fromTime = timestamp;
		CHKVOID(sdr_begin_xn(sdr));
		elt = sm_rbt_search(ionwm, vdb->rangeIndex,
				rfx_order_ranges, &arg2, &nextElt);
		if (elt)
		{
			addr = sm_rbt_data(ionwm, elt);
			oK(rfx_print_range(addr, buffer));
			printText(buffer);
		}
		else
		{
			printText("Range not found in database.");
		}

		sdr_exit_xn(sdr);
		return;
	}

	SYNTAX_ERROR;
}

static void	executeList(int tokenCount, char **tokens)
{
	Sdr		sdr = getIonsdr();
	PsmPartition	ionwm = getIonwm();
	IonVdb		*vdb = getIonVdb();
	PsmAddress	elt;
	PsmAddress	addr;
	char		buffer[RFX_NOTE_LEN];

	if (tokenCount < 2)
	{
		printText("List what?");
		return;
	}

	if (strcmp(tokens[1], "contact") == 0)
	{
		CHKVOID(sdr_begin_xn(sdr));
		for (elt = sm_rbt_first(ionwm, vdb->contactIndex); elt;
				elt = sm_rbt_next(ionwm, elt))
		{
			addr = sm_rbt_data(ionwm, elt);
			rfx_print_contact(addr, buffer);
			printText(buffer);
		}

		sdr_exit_xn(sdr);
		return;
	}

	if (strcmp(tokens[1], "range") == 0)
	{
		CHKVOID(sdr_begin_xn(sdr));
		for (elt = sm_rbt_first(ionwm, vdb->rangeIndex); elt;
				elt = sm_rbt_next(ionwm, elt))
		{
			addr = sm_rbt_data(ionwm, elt);
			rfx_print_range(addr, buffer);
			printText(buffer);
		}

		sdr_exit_xn(sdr);
		return;
	}

	SYNTAX_ERROR;
}

void initializeNode(int tokenCount, char **tokens)
{
	char		*ownNodeNbrString = tokens[1];
	IonParms	parms;

	if (tokenCount < 2 || *ownNodeNbrString == '\0')
	{
		return;
	}

	if (ionInitialize(&parms, strtouvast(ownNodeNbrString)) < 0)
	{
		putErrmsg("ionadmin can't initialize ION.", NULL);
	}
}

int	processLine(char *line, int lineLength)
{
	int		tokenCount;
	char		*cursor;
	int		i;
	char		*tokens[9];
	char		buffer[80];
	time_t		refTime;
	time_t		currentTime;

	tokenCount = 0;
	for (cursor = line, i = 0; i < 9; i++)
	{
		if (*cursor == '\0')
		{
			tokens[i] = NULL;
		}
		else
		{
			findToken(&cursor, &(tokens[i]));
			tokenCount++;
		}
	}

	if (tokenCount == 0)
	{
		return 0;
	}

	/*	Skip over any trailing whitespace.			*/

	while (isspace((int) *cursor))
	{
		cursor++;
	}

	/*	Make sure we've parsed everything.			*/

	if (*cursor != '\0')
	{
		printText("Too many tokens.");
		return 0;
	}

	/*	Have parsed the command.  Now execute it.		*/

	switch (*(tokens[0]))		/*	Command code.		*/
	{
		case 0:			/*	Empty line.		*/
		case '#':		/*	Comment.		*/
			return 0;

		case '?':
		case 'h':
			//printUsage();
			return 0;

		case 'v':
			snprintf(buffer, sizeof buffer, "%s",
					IONVERSIONNUMBER);
			printText(buffer);
			return 0;

		case '1':
			initializeNode(tokenCount, tokens);
			return 0;


		case '@':
			if (tokenCount < 2)
			{
				printText("Can't set reference time: \
no time.");
			}
			else if (strcmp(tokens[1], "0") == 0)
			{
				/*	Set reference time to
				 *	the current time.	*/

				currentTime = getUTCTime();
				oK(_referenceTime(&currentTime));
			}
			else
			{
				/*	Get current ref time.	*/

				refTime = _referenceTime(NULL);

				/*	Get new ref time, which
				 *	may be an offset from
				 *	the current ref time.	*/

				refTime = readTimestampUTC
						(tokens[1], refTime);

				/*	Record new ref time
				 *	for use by subsequent
				 *	command lines.		*/

				oK(_referenceTime(&refTime));
			}

			return 0;

		case 'a':
			executeAdd(tokenCount, tokens);

			return 0;

		case 'd':
			executeDelete(tokenCount, tokens);
			return 0;

		case 'i':
			executeInfo(tokenCount, tokens);
			return 0;

		case 'l':
			executeList(tokenCount, tokens);

			return 0;

		case 'm':
			//executeManage(tokenCount, tokens);

			return 0;

		case 'r':
			//executeRun(tokenCount, tokens);
			return 0;

		case 'e':
			//switchEcho(tokenCount, tokens);
			return 0;

		case 't':
			//exit(ion_is_up(tokenCount, tokens));

		case 'q':
			return -1;	/*	End program.		*/

		default:
			printText("Invalid command.  Enter '?' for help.");
			return 0;
	}
}

int	runIonadmin(char *cmdFileName)
{
	time_t	currentTime;
	int	cmdFile;
	char	line[256];
	int	len;

	currentTime = getUTCTime();
	oK(_referenceTime(&currentTime));
	if (cmdFileName == NULL)		/*	Interactive.	*/
	{
		return -1; /* no stdin. */
	}
	else if (strcmp(cmdFileName, ".") == 0) /*	Shutdown.	*/
	{

	}
	else					/*	Scripted.	*/
	{
		cmdFile = iopen(cmdFileName, O_RDONLY, 0777);
		if (cmdFile < 0)
		{
			PERROR("Can't open command file");
		}
		else
		{
			while (1)
			{
				if (igets(cmdFile, line, sizeof line, &len)
						== NULL)
				{
					if (len == 0)
					{
						break;	/*	Loop.	*/
					}

					putErrmsg("igets failed.", NULL);
					break;		/*	Loop.	*/
				}

				if (len == 0
				|| line[0] == '#')	/*	Comment.*/
				{
					continue;
				}

				if (processLine(line, len))
				{
					break;	/*	Out of loop.	*/
				}
			}

			close(cmdFile);
		}
	}

	printText("Stopping ionadmin.");
	//ionDetach();
	return 0;
}
