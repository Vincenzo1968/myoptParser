/*
   Copyright (C) 2013 Vincenzo Lo Cicero

   Author: Vincenzo Lo Cicero.
   e-mail: vincenzo.locicero@libero.it
    
   From an idea by Leonardo Pozzati <e-mail: leon.pozzati@gmail.com>.
   
   Special thanks to Francesco Baro for code review and bug fixing.

   This file is part of myoptParserLib.

   myoptParserLib is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   myoptParserLib is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with myoptParserLib.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "myoptParser.h"

/*
myopt_ArgList : {T_SHORT | T_LONG | T_POSITIONAL | T_END_OPTIONS | T_ERROR};
*/

void myopt_ExtractFileName(const char *szPathName, char *szFileName)
{
	int k, i, x;
	char c;

	k = strlen(szPathName);

#ifdef __linux__ /* __gnu_linux__ */
	c = '/';
#else
	c = '\\';
#endif

	i = k - 1;
	while ( i >= 0 )
	{
		if ( szPathName[i] == c )
			break;
		i--;
	}
	i++;
	x = 0;
	for (;;)
	{
		szFileName[x] = szPathName[i];
		if (szFileName[x] == '\0' || szFileName[x] == '.' )
			break;
		x++;
		i++;
	}
	szFileName[x] = '\0';
}

int myopt_ParseArray(myopt_Parser_t parser, int argc, char* argv[])
{
	int x, y, z;
	int len;
	int bSpace;
	
#ifdef __linux__
	strcpy(parser->strExeName, argv[0]);
#else
	char strExeName[256];
	myopt_ExtractFileName(argv[0], strExeName);
	strcpy(parser->strExeName, strExeName);
#endif	
	
	len = 0;	
	x = 1;
	while ( x < argc )
	{
		y = strlen(argv[x]);
		len += y;
		for ( z = 0; z < y; z++ )
		{
			if ( argv[x][z] == ' ' || argv[x][z] == '\t' || argv[x][z] == '\n' )
			{
				len += 2;
				break;
			}
		}
		x++;
	}
	len += argc + 1; // Per gli spazi
	//len++;
	
	parser->strInput = (char*)malloc(sizeof(char) * len);
	if ( !(parser->strInput ) )
		return 0;
	parser->strInput[0] = '\0';

	x = 1;
	while ( x < argc )
	{
		bSpace = 0;
		y = strlen(argv[x]);		
		for ( z = 0; z < y; z++ )
		{
			if ( argv[x][z] == ' ' || argv[x][z] == '\t' || argv[x][z] == '\n' )
			{
				bSpace = 1;
				break;
			}
		}
		if ( bSpace )
			strcat(parser->strInput, "\"");
		strcat(parser->strInput, argv[x]);
		if ( bSpace )
			strcat(parser->strInput, "\"");		
		strcat(parser->strInput, " ");
		x++;
	}
		
	return myopt_Parse(parser, parser->strInput);
}

int myopt_Parse(myopt_Parser_t parser, const char *strInput)
{
	char strError[1024];
	int ret = 1;
	int len;
	int x, y;	
	myopt_ArgsList* pArgsList = NULL;
	char strOptionName[1024];
	int bOnlyFirst;
	int *countOptionsOfGroup = NULL;
	int32_t idGroup;
	myopt_ParserData md;
	
	if ( !parser )
		return 0;

	if ( parser->strInput == NULL )
	{
		len = strlen(strInput);
		parser->strInput = (char*)malloc(sizeof(char) * len + 1);
		if ( !(parser->strInput ) )
			return 0;
	}	
		
	md.m_Parser = parser;
	strcpy(parser->strInput, strInput);	
	
	myopt_MakeUsageString(md.m_Parser);
	
	myopt_InitToken(&(md.m_Token));
	myopt_GetNextToken(md.m_Parser, &(md.m_Token));

	while ( ret && md.m_Token.Type != T_EOL )
	{
		ret = myopt_ArgList(&md);
	}
	
	if ( ret )
	{
		countOptionsOfGroup = (int*)malloc(sizeof(int) * md.m_Parser->countGroups);
		if ( !countOptionsOfGroup )
		{
			strcat(md.m_Parser->strInternalErrors, "Error: insufficient memory.\n");
			md.m_Parser->countInternalErrors++;
			return 0;
		}
		for ( x = 0; x < md.m_Parser->countGroups; x++ )
			countOptionsOfGroup[x] = 0;
				
		for ( x = 0; x < md.m_Parser->countOptArgs; x++ )
		{
			if ( md.m_Parser->arrayOptArgs[x].countOccurrences > 0 )
			{
				idGroup = md.m_Parser->arrayOptArgs[x].idGroup;
				countOptionsOfGroup[idGroup]++;
			}
			
			myopt_MakeOptionName(md.m_Parser->arrayOptArgs[x].shortName, md.m_Parser->arrayOptArgs[x].longName, strOptionName);
			
			if ( md.m_Parser->arrayOptArgs[x].countOccurrences > 1 && md.m_Parser->arrayOptArgs[x].eMob == MOB_ERROR )
			{
				ret = 0;
				sprintf(strError, "Error: multiple occurrences(%d) of '%s' option.\n", md.m_Parser->arrayOptArgs[x].countOccurrences, strOptionName);
				strcat(md.m_Parser->strErrors, strError);
			}				
			
			if ( md.m_Parser->arrayOptArgs[x].countOccurrences > 0 )
			{
				if ( md.m_Parser->arrayOptArgs[x].nArgsMin == md.m_Parser->arrayOptArgs[x].nArgsMax && md.m_Parser->arrayOptArgs[x].nArgsMin != md.m_Parser->arrayOptArgs[x].countArgs )
				{
					if ( md.m_Parser->arrayOptArgs[x].eMob != MOB_APPEND )
					{
						ret = 0;
						sprintf(strError, "Error: option %s: wrong number of arguments: must be %d; found %d.\n", strOptionName, md.m_Parser->arrayOptArgs[x].nArgsMin, md.m_Parser->arrayOptArgs[x].countArgs);
						strcat(md.m_Parser->strErrors, strError);
					}
				}
				else if ( md.m_Parser->arrayOptArgs[x].nArgsMin < md.m_Parser->arrayOptArgs[x].nArgsMax )
				{
					if ( md.m_Parser->arrayOptArgs[x].countArgs < md.m_Parser->arrayOptArgs[x].nArgsMin || md.m_Parser->arrayOptArgs[x].countArgs > md.m_Parser->arrayOptArgs[x].nArgsMax )
					{
						ret = 0;
						sprintf(strError, "Error: option %s: wrong number of arguments: must be between %d and %d; found %d.\n", strOptionName, md.m_Parser->arrayOptArgs[x].nArgsMin, md.m_Parser->arrayOptArgs[x].nArgsMax, md.m_Parser->arrayOptArgs[x].countArgs);
						strcat(md.m_Parser->strErrors, strError);
					}
				}
			}
						
			if ( md.m_Parser->arrayOptArgs[x].countOccurrences < 1 && md.m_Parser->arrayOptArgs[x].bRequired )
			{
				ret = 0;
				sprintf(strError, "Error: option '-%c --%s' is required.\n", md.m_Parser->arrayOptArgs[x].shortName, md.m_Parser->arrayOptArgs[x].longName);
				strcat(md.m_Parser->strErrors, strError);
			}
			
			if ( md.m_Parser->arrayOptArgs[x].countArgs > 0 )
			{								
				md.m_Parser->arrayOptArgs[x].arrayArgs = (myopt_Argument*)malloc(sizeof(myopt_Argument) * md.m_Parser->arrayOptArgs[x].countArgs);
				if ( !(md.m_Parser->arrayOptArgs[x].arrayArgs) )
				{
					strcat(md.m_Parser->strInternalErrors, "Error: insufficient memory\n");
					md.m_Parser->countInternalErrors++;
					ret = 0;
					break;
				}
				
				pArgsList = md.m_Parser->arrayOptArgs[x].listArgs;
				y = 0;
				while( pArgsList != NULL )
				{
					md.m_Parser->arrayOptArgs[x].arrayArgs[y].Type = pArgsList->arg.Type;
					strcpy(md.m_Parser->arrayOptArgs[x].arrayArgs[y].strValue, pArgsList->arg.strValue);
					md.m_Parser->arrayOptArgs[x].arrayArgs[y].intValue = pArgsList->arg.intValue;					
					md.m_Parser->arrayOptArgs[x].arrayArgs[y].floatValue = pArgsList->arg.floatValue;
					y++;
					pArgsList = pArgsList->next;
				}
				myopt_ArgsListFree(md.m_Parser->arrayOptArgs[x].listArgs);
				md.m_Parser->arrayOptArgs[x].listArgs = NULL;				
								
				if ( md.m_Parser->arrayOptArgs[x].eMob == MOB_APPEND )
					bOnlyFirst = 1;
				else
					bOnlyFirst = 0;
				if ( !myopt_CheckTypes(&md, md.m_Parser->arrayOptArgs[x].arrayArgs, md.m_Parser->arrayOptArgs[x].countArgs, md.m_Parser->arrayOptArgs[x].strTypes, strOptionName, bOnlyFirst) )
					ret = 0;
			}
		}
		
		for ( x = 0; x < md.m_Parser->countGroups; x++ )
		{
			if ( countOptionsOfGroup[x] == 0 && md.m_Parser->arrayGroups[x].bRequired == true )
			{
				ret = 0;
				sprintf(strError, "Error: must be present at least one option of group %d '%s'.\n", x, md.m_Parser->arrayGroups[x].strDescription);
				strcat(md.m_Parser->strErrors, strError);							
			}
			else if ( countOptionsOfGroup[x] > 1 && md.m_Parser->arrayGroups[x].bMutuallyExclusive == true )
			{
				ret = 0;
				sprintf(strError, "Error: there is more than one option for the mutually exclusive group %d '%s'. It is allowed only one.\n", x, md.m_Parser->arrayGroups[x].strDescription);
				strcat(md.m_Parser->strErrors, strError);
			}			
		}
		free(countOptionsOfGroup);
		countOptionsOfGroup = NULL;		
		
		if ( md.m_Parser->nPosArgsMin == md.m_Parser->nPosArgsMax && md.m_Parser->nPosArgsMin != md.m_Parser->countPosArgs )
		{
			ret = 0;
			sprintf(strError, "Error: wrong number of positional arguments: must be %d; found %d.\n", md.m_Parser->nPosArgsMin, md.m_Parser->countPosArgs);
			strcat(md.m_Parser->strErrors, strError);			
		}		
		else if ( md.m_Parser->nPosArgsMin < md.m_Parser->nPosArgsMax /*&& md.m_Parser.countPosArgs > 1*/ )
		{
			if ( md.m_Parser->countPosArgs < md.m_Parser->nPosArgsMin || md.m_Parser->countPosArgs > md.m_Parser->nPosArgsMax )
			ret = 0;
			sprintf(strError, "Error: wrong number of positional arguments: must be between %d and %d; found %d.\n", md.m_Parser->nPosArgsMin, md.m_Parser->nPosArgsMax, md.m_Parser->countPosArgs);
			strcat(md.m_Parser->strErrors, strError);
		}
		else if ( md.m_Parser->countPosArgs < md.m_Parser->nPosArgsMin ) /* md.m_Parser->nPosArgsMin == OR_MORE */
		{
			ret = 0;
			sprintf(strError, "Error: wrong number of positional arguments: must be at least %d; found %d.\n", md.m_Parser->nPosArgsMin, md.m_Parser->countPosArgs);
			strcat(md.m_Parser->strErrors, strError);
		}
				
		if ( !myopt_CheckTypes(&md, md.m_Parser->arrayPosArgs, md.m_Parser->countPosArgs, md.m_Parser->strPosTypes, NULL, 0) )
			ret = 0;			
	}
		
	return ret;
}

int myopt_Match(myopt_TokenTypeEnum ExpectedToken, myopt_ParserData *pd)
{
	if ( pd->m_Token.Type == ExpectedToken )
	{
		myopt_GetNextToken(pd->m_Parser, &(pd->m_Token));
		return 1;
	}

	return 0;
}

// myopt_ArgList : {T_SHORT | T_LONG | T_POSITIONAL | T_END_OPTIONS | T_ERROR};
int myopt_ArgList(myopt_ParserData *pd)
{
	char strError[1024];	
	char strOption[MAX_LEN_STR];
	int countEndOptions = 0;
	int x, y;
	myopt_ArgsList* pArgsList = NULL;
	myopt_ArgsList argument;
	
	while ( pd->m_Token.Type == T_SHORT ||
			pd->m_Token.Type == T_LONG ||
			pd->m_Token.Type == T_POSITIONAL ||
			pd->m_Token.Type == T_END_OPTIONS ||
			pd->m_Token.Type == T_ERROR )
	{
		my_continue:
		if ( pd->m_Token.Type == T_EOL )
			return 1;
		switch( pd->m_Token.Type )
		{
		case T_SHORT:
		case T_LONG:		
			strcpy(strOption, pd->m_Token.str);
			if ( pd->m_Token.Type == T_SHORT )
			{		
				//printf("SHORT OPTION: %s\n", pd->m_Token.str);
				x = myopt_LookupShort(pd->m_Parser, pd->m_Token.str[1]);
			}
			else
			{
				//printf("LONG OPTION: %s\n", pd->m_Token.str);
				x = myopt_LookupLong(pd->m_Parser, pd->m_Token.str + 2);
			}
			if ( x < 0 )
				return 0;
			pd->m_Parser->arrayOptArgs[x].countOccurrences++;
			
			pArgsList = pd->m_Parser->arrayOptArgs[x].listArgs;
			
			if ( pd->m_Parser->arrayOptArgs[x].nArgsMax < 0 ) /* nArgsMax == OR_MORE*/
			{
				myopt_GetNextToken(pd->m_Parser, &(pd->m_Token));
				
				while ( pd->m_Token.Type == T_POSITIONAL )
				{
					argument.arg.Type = T_STRING;
					strcpy(argument.arg.strValue, pd->m_Token.str);
					argument.arg.intValue = 0;
					argument.arg.floatValue = 0;
					pArgsList = myopt_ArgsListAppend(&argument, pArgsList);
					if ( pArgsList == NULL )
					{
						strcat(pd->m_Parser->strInternalErrors, "Error: insufficient memory\n");
						pd->m_Parser->countInternalErrors++;
						return 0;
					}
					pd->m_Parser->arrayOptArgs[x].countArgs++;
					myopt_GetNextToken(pd->m_Parser, &(pd->m_Token));
				}
				if ( pd->m_Parser->arrayOptArgs[x].countArgs < pd->m_Parser->arrayOptArgs[x].nArgsMin )
				{
					sprintf(strError, "Error: option %s: wrong number of arguments: must be almost %d; found %d.\n", strOption, pd->m_Parser->arrayOptArgs[x].nArgsMin, pd->m_Parser->arrayOptArgs[x].countArgs);
					strcat(pd->m_Parser->strErrors, strError);
					myopt_ArgsListFree(pArgsList);
					pd->m_Parser->arrayOptArgs[x].listArgs = pArgsList = NULL;
					return 0;
				}
				pd->m_Parser->arrayOptArgs[x].listArgs = pArgsList;
				goto my_continue;
			}
			else
			{
				for ( y = 0; y < pd->m_Parser->arrayOptArgs[x].nArgsMax; y++ )
				{
					myopt_GetNextToken(pd->m_Parser, &(pd->m_Token));
					if ( pd->m_Token.Type != T_POSITIONAL )
					{
						pd->m_Parser->arrayOptArgs[x].listArgs = pArgsList;
						goto my_continue;
					}
					else
					{
						argument.arg.Type = T_STRING;
						strcpy(argument.arg.strValue, pd->m_Token.str);
						argument.arg.intValue = 0;
						argument.arg.floatValue = 0;
						if ( pd->m_Parser->arrayOptArgs[x].eMob == MOB_OVERRIDE )
						{
							if ( pArgsList == NULL )
							{
								pArgsList = myopt_ArgsListAppend(&argument, pArgsList);
								if ( pArgsList == NULL )
								{
									strcat(pd->m_Parser->strInternalErrors, "Error: insufficient memory\n");
									pd->m_Parser->countInternalErrors++;
									return 0;
								}								
							}
							else
							{
								pArgsList->arg.Type = argument.arg.Type;
								strcpy(pArgsList->arg.strValue, argument.arg.strValue);
								pArgsList->arg.intValue = argument.arg.intValue;
								pArgsList->arg.floatValue = argument.arg.floatValue;								
							}
							pd->m_Parser->arrayOptArgs[x].countArgs = 1;
						}
						else
						{
							pArgsList = myopt_ArgsListAppend(&argument, pArgsList);
							if ( pArgsList == NULL )
							{
								strcat(pd->m_Parser->strInternalErrors, "Error: insufficient memory\n");
								pd->m_Parser->countInternalErrors++;
								return 0;
							}
							pd->m_Parser->arrayOptArgs[x].countArgs++;
						}
					}
				}
				
				pd->m_Parser->arrayOptArgs[x].listArgs = pArgsList;
			}
			
			break;
		case T_POSITIONAL:
			//printf("POSITIONAL ARGUMENT: %s\n", pd->m_Token.str);
			if ( pd->m_Parser->countPosArgs >= MAX_OPTS )
			{
				pd->m_Parser->arrayPosArgs = (myopt_Argument*)realloc(pd->m_Parser->arrayPosArgs, (sizeof(myopt_Argument) * pd->m_Parser->countPosArgs) + MAX_OPTS);
				if ( !(pd->m_Parser->arrayPosArgs) )
				{
					strcat(pd->m_Parser->strInternalErrors, "Error: insufficient memory\n");
					pd->m_Parser->countInternalErrors++;
					return 0;					
				}
				for ( x = pd->m_Parser->countPosArgs; x < pd->m_Parser->countPosArgs + MAX_OPTS; x++)
				{
					pd->m_Parser->arrayPosArgs[x].Type = T_STRING;
					pd->m_Parser->arrayPosArgs[x].strValue[0] = '\0';
					pd->m_Parser->arrayPosArgs[x].intValue = 0;
					pd->m_Parser->arrayPosArgs[x].floatValue = 0;
				}					
			}
			pd->m_Parser->arrayPosArgs[pd->m_Parser->countPosArgs].Type = T_STRING;
			strcpy(pd->m_Parser->arrayPosArgs[pd->m_Parser->countPosArgs].strValue, pd->m_Token.str);
			pd->m_Parser->arrayPosArgs[pd->m_Parser->countPosArgs].intValue = 0;
			pd->m_Parser->arrayPosArgs[pd->m_Parser->countPosArgs].floatValue = 0;
			pd->m_Parser->countPosArgs++;
			break;
		case T_END_OPTIONS:
			countEndOptions++;
			if ( countEndOptions > 1 )
			{
				strcat(pd->m_Parser->strErrors, "Error: multiple occurrences of '--'.\n");
				return 0;
			}
			//printf("END OF OPTIONS: %s\n", pd->m_Token.str);
			break;
		case T_ERROR:
			return 0;
		default:
			strcat(pd->m_Parser->strErrors, "Error: unexpected token.\n");
			return 0;
		}		
		
		myopt_GetNextToken(pd->m_Parser, &(pd->m_Token));
	}

	return 1;
}

int myopt_GetNextType(const char *strTypes, int len, int *nextPos, int bOnlyFirst, int lastType)
{
	char str[MAX_LEN_STR];
	int x;
	int i;
	
	x = *nextPos;
	
	if ( bOnlyFirst )
		x = 0;
	
	if ( x >= len )
		return T_STRING;
	
	i = 0;
	while ( x < len )
	{
		if ( strTypes[x] != ',' )
			str[i++] = strTypes[x];
		else
			break;
		x++;
	}
	str[i] = '\0';
	x++;
	*nextPos = x;
	
	if ( strcmp(str, "int") == 0 )
		lastType = T_INT;
	else if ( strcmp(str, "float") == 0 )
		lastType = T_FLOAT;

	return lastType;
}

int myopt_GetValueType(myopt_Argument *arg)
{
	int x = 0;
	char c;
	
	enum States{S0, S1, S2, S3, S4, S5, S6, S7} state;
	
	state = S0;
	c = arg->strValue[x++];
	
	while ( c != '\0' )
	{		
		switch (state)
		{
			case S0:
				if ( c == '-' || c == '+' )
					state = S1;
				else if ( isdigit(c) )
					state = S2;
				else
					goto uscita;
				break;
			case S1:
				if ( isdigit(c) )
					state = S2;
				else if ( c == '.' )
					state = S3;
				else
					goto uscita;
				break;
			case S2:
				if ( isdigit(c) )
					state = S2;
				else if ( c == '.' )
					state = S3;			
				else if ( c == 'e' || c == 'E' )
					state = S5;
				else
					goto uscita;
				break;
			case S3:
				if ( isdigit(c) )
					state = S4;
				else
					goto uscita;
				break;
			case S4:
				if ( isdigit(c) )
					state = S4;
				else if ( c == 'e' || c == 'E' )
					state = S5;
				else
					goto uscita;								
				break;
			case S5:
				if ( c == '-' || c == '+' )
					state = S6;
				else if ( isdigit(c) )
					state = S7;
				else
					goto uscita;			
				break;
			case S6:
				if ( isdigit(c) )
					state = S7;
				else
					goto uscita;						
				break;
			case S7:
				if ( isdigit(c) )
					state = S7;
				else
					goto uscita;
				break;
		}		
		c = arg->strValue[x++];
	}
	
uscita:
	switch ( state )
	{
		case S2:
			arg->Type = T_INT;
			arg->intValue = atoi(arg->strValue);
			return T_INT;		
		case S4:
		case S7:
			arg->Type = T_FLOAT;
			arg->floatValue = atof(arg->strValue);
			return T_FLOAT;
		default:
			return T_STRING;
	}
	
	return T_STRING;
}

int myopt_CheckTypes(myopt_ParserData *pd, myopt_Argument arrayArgs[], int32_t countPosArgs, char *strTypes, const char *strOptName, int bOnlyFirst)
{
	char strError[1024];	
	int x;
	int type1, type2;
	char strType1[MAX_LEN_STR];
	char strType2[MAX_LEN_STR];	
	int len, nNextPos;
	int ret = 1;
	
	myopt_NormalizeStringTypes(strTypes);
	len = strlen(strTypes);
	nNextPos = 0;
	
	x = 0;
	type1 = T_STRING;
	while ( x < countPosArgs )
	{
		type1 = myopt_GetNextType(strTypes, len, &nNextPos, bOnlyFirst, type1);
		type2 = myopt_GetValueType(&(arrayArgs[x]));
		
		switch ( type1 )
		{
			case T_STRING:
				strcpy(strType1, "string");
				break;
			case T_INT:
				strcpy(strType1, "int");			
				break;
			case T_FLOAT:
				strcpy(strType1, "float");
				break;				
		}

		switch ( type2 )
		{
			case T_STRING:
				strcpy(strType2, "string");
				break;
			case T_INT:
				strcpy(strType2, "int");			
				break;
			case T_FLOAT:
				strcpy(strType2, "float");
				break;				
		}
		
		if ( type1 != type2 )
		{
			if ( type1 == T_STRING )
			{
				arrayArgs[x].Type = T_STRING;
			}
			else if ( type1 == T_FLOAT && type2 == T_INT )
			{
				arrayArgs[x].Type = T_FLOAT;
				arrayArgs[x].floatValue = (double)arrayArgs[x].intValue;
			}
			else
			{
				if ( strOptName == NULL || 	strlen(strOptName) == 0 )
					sprintf(strError, "Error: positional argument %d wrong type. Must be %s; found %s: '%s'\n", x + 1, strType1, strType2, arrayArgs[x].strValue);
				else
					sprintf(strError, "Error: option '%s': argument %d wrong type. Must be %s; found %s: '%s'\n", strOptName, x + 1, strType1, strType2, arrayArgs[x].strValue);
				strcat(pd->m_Parser->strErrors, strError);
				ret = 0;
			}
		}
						
		x++;
	}
		
	return ret;
}
