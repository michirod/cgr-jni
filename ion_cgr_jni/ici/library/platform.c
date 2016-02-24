/*
 * platform.c
 *
 *  Created on: 18 nov 2015
 *      Author: michele
 */

#include "platform.h"

#include <stdio.h>
#include <stdarg.h>

#define isprintf snprintf

void			microsnooze(unsigned int m)
{
	// TODO stub
}

void	isignal(int sig, void (*func)(int))
{
}

void writeErrmsgMemos()
{

}
void	_putErrmsg(const char *fileName, int lineNbr, const char *text,
		const char *arg)
{
	//TODO stub
	fprintf(stderr, "Error in %s:%d, %s: %s\n", fileName, lineNbr, text, arg);
	fflush(stderr);
	//writeErrmsgMemos();
}
void			_putSysErrmsg(const char * fileName, int lineNbr, const char * text,
		const char * arg){
	_putErrmsg(fileName, lineNbr, text, arg);
}

int	_iEnd(const char *fileName, int lineNbr, const char *arg)
{
	return 1;
}

char *igets(int fd, char * buffer, int buflen, int * lineLen)
{
	int i, res;
	char c;
	for (i = 0; i < buflen; i++)
	{
		res = read(fd, &c, 1);
		if (res < 0)
		{
			*lineLen = 0;
			return NULL;
		}
		if (res == 0 || c == '\n')
			break;
		buffer[i] = c;
	}
	if (res == 0 && i == 0)
	{
		*lineLen = 0;
		return NULL;
	}
	if (i == buflen)
		buffer[i - 1] = '\0';
	else
		buffer[i] = '\0';
	*lineLen = strlen(buffer);
	return buffer;
}

int	iputs(int fd, char * string)
{
	int i = 0;
	while (string[i] != '\0')
	{
		if (write(fd, &(string[i]), 1) != 1)
			return -1;
		i++;
	}
	return i;
}

char	*uToa(unsigned int arg)
{
	//TODO stub
	static char	utoa_str[33];

	snprintf(utoa_str, sizeof utoa_str, "%u", arg);
	return utoa_str;
}
char *iToa(int arg)
{
	static char	itoa_str[33];

	snprintf(itoa_str, sizeof itoa_str, "%d", arg);
	return itoa_str;
}
char * istrcpy(char * to, const char * from, size_t size)
{
	return strncpy(to, from, size);
}
static void	logToStdout(char *text)
{

#ifdef DEBUG_PRINTS
	if (text)
	{
		fprintf(stdout, "%s\n", text);
		fflush(stdout);
	}
#endif
}

void	writeMemo(char *text)
{

	if (text)
	{
		logToStdout(text);
	}
}
void	writeMemoNote(char *text, char *note)
{
	char	*noteText = note ? note : "";
	char	textBuffer[1024];

	if (text)
	{
		isprintf(textBuffer, sizeof textBuffer, "%.900s: %.64s",
				text, noteText);
		//(_logOneMessage(NULL))(textBuffer);
		logToStdout(textBuffer);
	}
}

void	findToken(char **cursorPtr, char **token)
{
	char	*cursor;

	if (token == NULL)
	{
		//ABORT_AS_REQD;
		return;
	}

	*token = NULL;		/*	The default.			*/
	if (cursorPtr == NULL || (*cursorPtr) == NULL)
	{
		//ABORT_AS_REQD;
		return;
	}

	cursor = *cursorPtr;

	/*	Skip over any leading whitespace.			*/

	while (isspace((int) *cursor))
	{
		cursor++;
	}

	if (*cursor == '\0')	/*	Nothing but whitespace.		*/
	{
		*cursorPtr = cursor;
		return;
	}

	/*	Token delimited by quotes is the complicated case.	*/

	if (*cursor == '\'')	/*	Quote-delimited token.		*/
	{
		/*	Token is everything after this single quote,
		 *	up to (but not including) the next non-escaped
		 *	single quote.					*/

		cursor++;
		while (*cursor != '\0')
		{
			if (*token == NULL)
			{
				*token = cursor;
			}

			if (*cursor == '\\')	/*	Escape.		*/
			{
				/*	Include the escape character
				 *	plus the following (escaped)
				 *	character (unless it's the end
				 *	of the string) in the token.	*/

				cursor++;
				if (*cursor == '\0')
				{
					*cursorPtr = cursor;
					return;	/*	unmatched quote	*/
				}

				cursor++;
				continue;
			}

			if (*cursor == '\'')	/*	End of token.	*/
			{
				*cursor = '\0';
				cursor++;
				*cursorPtr = cursor;
				return;		/*	matched quote	*/
			}

			cursor++;
		}

		/*	If we get here it's another case of unmatched
		 *	quote, but okay.				*/

		*cursorPtr = cursor;
		return;
	}

	/*	The normal case: a simple whitespace-delimited token.
	 *	Token is this character and all successive characters
	 *	up to (but not including) the next whitespace.		*/

	*token = cursor;
	cursor++;
	while (*cursor != '\0')
	{
		if (isspace((int) *cursor))	/*	End of token.	*/
		{
			*cursor = '\0';
			cursor++;
			break;
		}

		cursor++;
	}

	*cursorPtr = cursor;
}

void	loadScalar(Scalar *s, signed int i)
{
	CHKVOID(s);
	if (i < 0)
	{
		i = 0 - i;
	}

	s->gigs = 0;
	s->units = i;
	while (s->units >= ONE_GIG)
	{
		s->gigs++;
		s->units -= ONE_GIG;
	}
}

void	increaseScalar(Scalar *s, signed int i)
{
	CHKVOID(s);
	if (i < 0)
	{
		i = 0 - i;
	}

	while (i >= ONE_GIG)
	{
		i -= ONE_GIG;
		s->gigs++;
	}

	s->units += i;
	while (s->units >= ONE_GIG)
	{
		s->gigs++;
		s->units -= ONE_GIG;
	}
}

void	reduceScalar(Scalar *s, signed int i)
{
	CHKVOID(s);
	if (i < 0)
	{
		i = 0 - i;
	}

	while (i >= ONE_GIG)
	{
		i -= ONE_GIG;
		s->gigs--;
	}

	while (i > s->units)
	{
		s->units += ONE_GIG;
		s->gigs--;
	}

	s->units -= i;
}

void	multiplyScalar(Scalar *s, signed int i)
{
	double	product;

	CHKVOID(s);
	if (i < 0)
	{
		i = 0 - i;
	}

	product = ((((double)(s->gigs)) * ONE_GIG) + (s->units)) * i;
	s->gigs = (int) (product / ONE_GIG);
	s->units = (int) (product - (((double)(s->gigs)) * ONE_GIG));
}

void	divideScalar(Scalar *s, signed int i)
{
	double	quotient;

	CHKVOID(s);
	CHKVOID(i != 0);
	if (i < 0)
	{
		i = 0 - i;
	}

	quotient = ((((double)(s->gigs)) * ONE_GIG) + (s->units)) / i;
	s->gigs = (int) (quotient / ONE_GIG);
	s->units = (int) (quotient - (((double)(s->gigs)) * ONE_GIG));
}

void	copyScalar(Scalar *to, Scalar *from)
{
	CHKVOID(to);
	CHKVOID(from);
	to->gigs = from->gigs;
	to->units = from->units;
}

void	addToScalar(Scalar *s, Scalar *increment)
{
	CHKVOID(s);
	CHKVOID(increment);
	increaseScalar(s, increment->units);
	s->gigs += increment->gigs;
}

void	subtractFromScalar(Scalar *s, Scalar *decrement)
{
	CHKVOID(s);
	CHKVOID(decrement);
	reduceScalar(s, decrement->units);
	s->gigs -= decrement->gigs;
}

int	scalarIsValid(Scalar *s)
{
	CHKZERO(s);
	return (s->gigs >= 0);
}
