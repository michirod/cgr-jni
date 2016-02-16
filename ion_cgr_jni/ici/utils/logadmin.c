/*

	logadmin.c:	contact list adminstration interface.

									*/

#include "zco.h"
#include "rfx.h"

static time_t	_referenceTime(time_t *newValue)
{
	static time_t	reftime = 0;
	
	if (newValue)
	{
		reftime = *newValue;
	}

	return reftime;
}

static int	_echo(int *newValue)
{
	static int	state = 0;
	
	if (newValue)
	{
		if (*newValue == 1)
		{
			state = 1;
		}
		else
		{
			state = 0;
		}
	}

	return state;
}

static void	printText(char *text)
{
	if (_echo(NULL))
	{
		writeMemo(text);
	}

	PUTS(text);
}

static void	handleQuit()
{
	printText("Please enter command 'q' to stop the program.");
}

static void	printSyntaxError(int lineNbr)
{
	char	buffer[80];

	isprintf(buffer, sizeof buffer, "Syntax error at line %d of logadmin.c",
			lineNbr);
	printText(buffer);
}

#define	SYNTAX_ERROR	printSyntaxError(__LINE__)

static void	printUsage()
{
	PUTS("Valid commands are:");
	PUTS("\tq\tQuit");
	PUTS("\th\tHelp");
	PUTS("\t?\tHelp");
	PUTS("\t@\tAt");
	PUTS("\t   @ <reference time>");
	PUTS("\t\tTime format is either +ss or yyyy/mm/dd-hh:mm:ss,");
	PUTS("\t\tor to set reference time to the current time use '@ 0'.");
	PUTS("\t\tTo specify a NEGATIVE relative time, use +-ss.");
	PUTS("\t\tThe @ command sets the reference time from which subsequent \
relative times (+ss) are computed.");
	PUTS("\ta\tAdd");
	PUTS("\t   a contact <from time> <until time> <from node#> <to node#> \
<xmit rate in bytes per second> <index>");
	PUTS("\t\tTime format is either +ss or yyyy/mm/dd-hh:mm:ss.");
	PUTS("\t\tIndex is 0 if contact is reported by sender, else 1.");
	PUTS("\tl\tList");
	PUTS("\t   l contact <index>");
	PUTS("\te\tEnable or disable echo of printed output to log file");
	PUTS("\t   e { 0 | 1 }");
	PUTS("\t#\tComment");
	PUTS("\t   # <comment text>");
}

static void	executeAdd(int tokenCount, char **tokens)
{
	time_t		refTime;
	time_t		fromTime;
	time_t		toTime;
	uvast		fromNodeNbr;
	uvast		toNodeNbr;
	unsigned int	xmitRate;
	int		idx;

	if (tokenCount < 2)
	{
		printText("Add what?");
		return;
	}

	if (tokenCount != 8)
	{
		SYNTAX_ERROR;
		return;
	}

	refTime = _referenceTime(NULL);
	fromTime = readTimestampUTC(tokens[2], refTime);
	toTime = readTimestampUTC(tokens[3], refTime);
	if (toTime <= fromTime)
	{
		printText("Interval end time must be later than start time \
and earlier than 19 January 2038.");
		return;
	}

	fromNodeNbr = strtouvast(tokens[4]);
	toNodeNbr = strtouvast(tokens[5]);
	if (strcmp(tokens[1], "contact") == 0)
	{
		xmitRate = strtol(tokens[6], NULL, 0);
		idx = strtol(tokens[7], NULL, 0);
		oK(rfx_log_discovered_contact(fromTime, toTime, fromNodeNbr,
				toNodeNbr, xmitRate, idx));
		return;
	}

	SYNTAX_ERROR;
}

static void	executeList(int tokenCount, char **tokens)
{
	Sdr		sdr = getIonsdr();
	Object		dbobj = getIonDbObject();
	IonDB		db;
	int		idx;
	Object		elt;
	Object		addr;
	PastContact	entry;
	char		fromTimeBuffer[TIMESTAMPBUFSZ];
	char		toTimeBuffer[TIMESTAMPBUFSZ];
	char		buffer[256];

	if (tokenCount < 2)
	{
		printText("List what?");
		return;
	}

	if (strcmp(tokens[1], "contact") == 0)
	{
		idx = strtol(tokens[2], NULL, 0);
		CHKVOID(sdr_begin_xn(sdr));
		sdr_read(sdr, (char *) &db, dbobj, sizeof(IonDB));
		for (elt = sdr_list_first(sdr, db.contactLog[idx]); elt;
				elt = sdr_list_next(sdr, elt))
		{
			addr = sdr_list_data(sdr, elt);
			sdr_read(sdr, (char *) &entry, addr,
					sizeof(PastContact));
			writeTimestampUTC(entry.fromTime, fromTimeBuffer);
			writeTimestampUTC(entry.toTime, toTimeBuffer);
			isprintf(buffer, sizeof buffer, "From %20s to %20s \
the xmit rate from node " UVAST_FIELDSPEC " to node " UVAST_FIELDSPEC " was \
 %10lu bytes/sec.", fromTimeBuffer, toTimeBuffer, entry.fromNode,
					entry.toNode, entry.xmitRate);
			//rfx_print_contact(addr, buffer);
			printText(buffer);
		}

		sdr_exit_xn(sdr);
		return;
	}

	SYNTAX_ERROR;
}

static void	switchEcho(int tokenCount, char **tokens)
{
	int	state;

	if (tokenCount < 2)
	{
		printText("Echo on or off?");
		return;
	}

	switch (*(tokens[1]))
	{
	case '0':
		state = 0;
		break;

	case '1':
		state = 1;
		break;

	default:
		printText("Echo on or off?");
		return;
	}

	oK(_echo(&state));
}

static int	processLine(char *line, int lineLength)
{
	int		tokenCount;
	char		*cursor;
	int		i;
	char		*tokens[9];
	char		buffer[80];
	time_t		refTime;
	time_t		currentTime;
	struct timeval	done_time;
	struct timeval	cur_time;

	int max = 0;
	int count = 0;

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
			printUsage();
			return 0;

		case '@':
			if (ionAttach() == 0)
			{
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
			}

			return 0;

		case 'a':
			if (ionAttach() == 0)
			{
				executeAdd(tokenCount, tokens);
			}

			return 0;

		case 'l':
			if (ionAttach() == 0)
			{
				executeList(tokenCount, tokens);
			}

			return 0;

		case 'e':
			switchEcho(tokenCount, tokens);
			return 0;

		case 'q':
			return -1;	/*	End program.		*/

		default:
			printText("Invalid command.  Enter '?' for help.");
			return 0;
	}
}

int	runLogadmin(char *cmdFileName)
{
	time_t	currentTime;
	int	cmdFile;
	char	line[256];
	int	len;

	currentTime = getUTCTime();
	oK(_referenceTime(&currentTime));
	if (cmdFileName == NULL)		/*	Interactive.	*/
	{
#ifdef FSWLOGGER
		return 0;			/*	No stdin.	*/
#else
		cmdFile = fileno(stdin);
		isignal(SIGINT, handleQuit);
		while (1)
		{
			printf(": ");
			fflush(stdout);
			if (igets(cmdFile, line, sizeof line, &len) == NULL)
			{
				if (len == 0)
				{
					break;
				}

				putErrmsg("igets failed.", NULL);
				break;		/*	Out of loop.	*/
			}

			if (len == 0)
			{
				continue;
			}

			if (processLine(line, len))
			{
				break;		/*	Out of loop.	*/
			}
		}
#endif
	}
	else if (strcmp(cmdFileName, ".") == 0) /*	Shutdown.	*/
	{
		if (ionAttach() == 0)
		{
			rfx_stop();
		}
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

	writeErrmsgMemos();
	printText("Stopping logadmin.");
	ionDetach();
	return 0;
}

#if defined (ION_LWT)
int	logadmin(int a1, int a2, int a3, int a4, int a5,
		int a6, int a7, int a8, int a9, int a10)
{
	char	*cmdFileName = (char *) a1;
#endif
