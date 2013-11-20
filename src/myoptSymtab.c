/*
   Copyright (C) 2013 Vincenzo Lo Cicero

   Author: Vincenzo Lo Cicero.
   e-mail: vincenzo.locicero@libero.it
    
   From an idea by Leonardo Pozzati <e-mail: leon.pozzati@gmail.com>.

   Special thanks to Francesco Baro <e-mail: francesco.baro@libero.it >
   for code review and bug fixing.
   
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

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "myoptSymtab.h"

myopt_Parser_t myopt_InitParser()
{
	int x;
	myopt_Parser_t parser = NULL;
	
	setlocale(LC_ALL, "");
	
	/* printf("\nInitParser : PACKAGE: %s LOCALEDIR: %s\n", PACKAGE, LOCALEDIR); */
 
/* #ifdef ENABLE_NLS */
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
/* #endif */
	
	parser = (myopt_Parser_t)malloc(sizeof(struct _myopt_Parser_t));
	if ( !parser )
		return NULL;
		
	parser->strInput = NULL;
	
	parser->strExeName[0] = '\0';
		
	parser->strErrors = (char*)malloc(sizeof(char) * STR_ERRORS_SIZE);
	if ( !parser->strErrors )
	{
		free(parser);
		parser = NULL;
		return NULL;
	}
	parser->strErrors[0] = '\0';
	
	parser->strInternalErrors = (char*)malloc(sizeof(char) * STR_ERRORS_SIZE);
	if ( !parser->strInternalErrors )
	{
		free(parser->strErrors);
		parser->strErrors = NULL;				
		free(parser);
		parser = NULL;
		return NULL;
	}
	parser->strInternalErrors[0] = '\0';
	parser->countInternalErrors = 0;	
	
	parser->strUsage = (char*)malloc(sizeof(char) * STR_ERRORS_SIZE);
	if ( !parser->strUsage )
	{
		free(parser->strErrors);
		parser->strErrors = NULL;
		free(parser->strInternalErrors);
		parser->strInternalErrors = NULL;
		free(parser);
		parser = NULL;
		return NULL;
	}
	parser->strUsage[0] = '\0';
		
	parser->arrayGroups = (myopt_Group*)malloc(sizeof(myopt_Group) * MAX_OPTS);
	if ( !parser->arrayGroups )
	{
		free(parser->strErrors);
		parser->strErrors = NULL;
		free(parser->strInternalErrors);
		parser->strInternalErrors = NULL;
		free(parser->strUsage);
		parser->strUsage = NULL;		
		free(parser);
		parser = NULL;
		return NULL;
	}
	parser->countGroups = 0;
	for ( x = 0; x < MAX_OPTS; x++)
	{
		parser->arrayGroups[x].strDescription[0] = '\0';
		parser->arrayGroups[x].bRequired = false;
		parser->arrayGroups[x].bMutuallyExclusive = false;
	}
		
	parser->arrayOptArgs = (myopt_Option*)malloc(sizeof(myopt_Option) * MAX_OPTS);
	if ( !parser->arrayOptArgs )
	{
		free(parser->strErrors);
		parser->strErrors = NULL;
		free(parser->strInternalErrors);
		parser->strInternalErrors = NULL;
		free(parser->strUsage);
		parser->strUsage = NULL;				
		free(parser->arrayGroups);
		parser->arrayGroups = NULL;		
		free(parser);
		parser = NULL;
		return NULL;
	}
	parser->countOptArgs = 0;
	for ( x = 0; x < MAX_OPTS; x++)
	{
		parser->arrayOptArgs[x].shortName = '\0';
		parser->arrayOptArgs[x].longName[0] = '\0';
		parser->arrayOptArgs[x].strDescription[0] = '\0';		
		parser->arrayOptArgs[x].strTypes[0] = '\0';
		parser->arrayOptArgs[x].arrayArgs = NULL;
		parser->arrayOptArgs[x].countArgs = 0;
		parser->arrayOptArgs[x].countOccurrences = 0;
	}
			
	parser->arrayPosArgs = (myopt_Argument*)malloc(sizeof(myopt_Argument) * MAX_OPTS);
	if ( !parser->arrayPosArgs )
	{
		free(parser->strErrors);
		parser->strErrors = NULL;
		free(parser->strInternalErrors);
		parser->strInternalErrors = NULL;
		free(parser->strUsage);
		parser->strUsage = NULL;				
		free(parser->arrayGroups);
		parser->arrayGroups = NULL;				
		free(parser->arrayOptArgs);
		parser->arrayOptArgs = NULL;
		free(parser);
		parser = NULL;
		return NULL;
	}
	parser->countPosArgs = 0;
	for ( x = 0; x < MAX_OPTS; x++)
	{
		parser->arrayPosArgs[x].Type = T_STRING;
		parser->arrayPosArgs[x].strValue[0] = '\0';
		parser->arrayPosArgs[x].intValue = 0;
		parser->arrayPosArgs[x].floatValue = 0;
	}
	
	strncpy(parser->strPosArgsName, "POS_ARG", MAX_LEN_STR);
	parser->nPosArgsMin = 0;	
	parser->nPosArgsMax = OR_MORE;
	parser->strPosTypes[0] = '\0';
		
	return parser;
}

void myopt_FreeParser(myopt_Parser_t parser)
{
	int x;
	
	if ( parser == NULL )
		return;

	if ( parser->strInput != NULL )
	{
		free(parser->strInput);
		parser->strInput = NULL;
	}
				
	if ( parser->strErrors != NULL )
	{
		free(parser->strErrors);
		parser->strErrors = NULL;
	}

	if ( parser->strInternalErrors != NULL )
	{
		free(parser->strInternalErrors);
		parser->strInternalErrors = NULL;
	}

	if ( parser->strUsage != NULL )
	{	
		free(parser->strUsage);
		parser->strUsage = NULL;		
	}
		
	if ( parser->arrayGroups != NULL )
	{
		free(parser->arrayGroups);
		parser->arrayGroups = NULL;
	}		
	
	if ( parser->arrayOptArgs != NULL )
	{
		for ( x = 0; x < parser->countOptArgs; x++)
		{
			if ( parser->arrayOptArgs[x].arrayArgs != NULL )
			{
				free(parser->arrayOptArgs[x].arrayArgs);
				parser->arrayOptArgs[x].arrayArgs = NULL;
			}
			
			if ( parser->arrayOptArgs[x].listArgs != NULL )
			{
				myopt_ArgsListFree(parser->arrayOptArgs[x].listArgs);
				parser->arrayOptArgs[x].listArgs = NULL;
			}
		}		
		free(parser->arrayOptArgs);
		parser->arrayOptArgs = NULL;
	}

	if ( parser->arrayPosArgs != NULL )
	{
		free(parser->arrayPosArgs);
		parser->arrayPosArgs = NULL;
	}	
}

int32_t myopt_AddGroup(myopt_Parser_t parser, const char *strDescription, bool bMutuallyExclusive, bool bRequired)
{
	int32_t ret;
	int32_t x;
	char strError[1024];	
			
	if ( !parser )
	{
		sprintf(strError, gettext("Invalid call myopt_AddGroup(%s): argument 'parser' is NULL\n"), strDescription);
		strncat(parser->strInternalErrors, strError, STR_ERRORS_SIZE);
		parser->countInternalErrors++;
		return -1;
	}		
	
	ret = parser->countGroups;

	if ( parser->countGroups >= MAX_OPTS )
	{
		parser->arrayGroups = (myopt_Group*)realloc(parser->arrayGroups, (sizeof(myopt_Group) * parser->countGroups) + MAX_OPTS);
		if ( !(parser->arrayGroups) )
		{
			strncat(parser->strInternalErrors, gettext(ERRMSG_INSUFFICIENT_MEMORY), STR_ERRORS_SIZE);
			parser->countInternalErrors++;
			return -1;				
		}
		for ( x = parser->countGroups; x < parser->countGroups + MAX_OPTS; x++)
		{
			parser->arrayGroups[x].strDescription[0] = '\0';
			parser->arrayGroups[x].bRequired = false;
			parser->arrayGroups[x].bMutuallyExclusive = false;
		}
	}
	
	if ( strDescription == NULL || strlen(strDescription) == 0 )
		sprintf(parser->arrayGroups[parser->countGroups].strDescription, gettext(MSG_GROUP), parser->countGroups);
	else
		strncpy(parser->arrayGroups[parser->countGroups].strDescription, strDescription, MAX_LEN_STR);

	parser->arrayGroups[parser->countGroups].bMutuallyExclusive = bMutuallyExclusive;	
	parser->arrayGroups[parser->countGroups].bRequired = bRequired;
		
	parser->countGroups++;
	
	return ret;
}

bool myopt_AddOption(myopt_Parser_t parser,
                     int32_t idGroup,
                     char shortName,
                     const char *longName,
                     const char *strDescription,                     
                     bool bRequired,
                     myopt_MOBEnum eMob,
                     int32_t nArgsMin,
                     int32_t nArgsMax,                     
                     const char *strTypes)
{
	char str[MAX_LEN_STR];
	char strError[1024];
	char strOptionName[1024];
	
	int32_t x;
	
	if ( shortName == 0 )
		shortName = '\0';
	
	if ( !myopt_MakeOptionName(shortName, longName, strOptionName) )
	{
		strncat(parser->strInternalErrors, gettext("Error: must be specified at least one of shortName and longName\n"), STR_ERRORS_SIZE);
		parser->countInternalErrors++;
		return false;
	}
		
	if ( !parser )
	{
		sprintf(strError, gettext("Invalid call myopt_AddOption(%s): argument 'parser' is NULL\n"), strOptionName);
		strncat(parser->strInternalErrors, strError, STR_ERRORS_SIZE);
		return false;
	}
		
	if ( idGroup < 0 || idGroup >= parser->countGroups )
	{
		sprintf(strError, gettext("Invalid call myopt_AddOption(%s): idGroup %d not valid.\n"), strOptionName, idGroup);
		strncat(parser->strInternalErrors, strError, STR_ERRORS_SIZE);
		parser->countInternalErrors++;
		return false;
	}
		
	if ( parser->countOptArgs >= MAX_OPTS )
	{
		parser->arrayOptArgs = (myopt_Option*)realloc(parser->arrayOptArgs, (sizeof(myopt_Option) * parser->countOptArgs) + MAX_OPTS);
		if ( !(parser->arrayOptArgs) )
		{
			strncat(parser->strInternalErrors, gettext(ERRMSG_INSUFFICIENT_MEMORY), STR_ERRORS_SIZE);
			parser->countInternalErrors++;
			return false;				
		}
		for ( x = parser->countOptArgs; x < parser->countOptArgs + MAX_OPTS; x++)
		{
			parser->arrayOptArgs[x].shortName = '\0';
			parser->arrayOptArgs[x].longName[0] = '\0';
			parser->arrayOptArgs[x].strTypes[0] = '\0';
			parser->arrayOptArgs[x].arrayArgs = NULL;
			parser->arrayOptArgs[x].countArgs = 0;
			parser->arrayOptArgs[x].countOccurrences = 0;
			parser->arrayOptArgs[x].listArgs = NULL;
		}					
	}
		
	parser->arrayOptArgs[parser->countOptArgs].idGroup = idGroup;	
	
	if ( shortName == 0 && longName == NULL )
	{
		sprintf(strError, gettext("Invalid call myopt_AddOption(%s): must be specified one of shortName and longName\n"), strOptionName);
		strncat(parser->strInternalErrors, strError, STR_ERRORS_SIZE);
		parser->countInternalErrors++;		
		return false;		
	}
	
	if ( shortName != 0 )
	{
		if ( myopt_LookupShort(parser, shortName) >= 0 )
		{
			sprintf(strError, gettext("Invalid call myopt_AddOption(%s): the option is already present in the symbol table\n"), strOptionName);
			strncat(parser->strInternalErrors, strError, STR_ERRORS_SIZE);
			parser->countInternalErrors++;		
			return false;
		}
		else
		{
			parser->arrayOptArgs[parser->countOptArgs].shortName = shortName;
		}
	}
	else
	{
		parser->arrayOptArgs[parser->countOptArgs].shortName = '\0';
	}
								
	if ( longName == NULL || strlen(longName) == 0 )
	{
		parser->arrayOptArgs[parser->countOptArgs].longName[0] = '\0';
	}
	else
	{		
		/* if ( myopt_LookupLong(parser, longName) >= 0 ) */
		if ( myopt_FindLong(parser, longName) >= 0 )
		{
			sprintf(strError, gettext("Invalid call myopt_AddOption(%s): the option is already present in the symbol table\n"), strOptionName);
			strncat(parser->strInternalErrors, strError, STR_ERRORS_SIZE);
			parser->countInternalErrors++;					
			return false;
		}
		else
		{
			strncpy(parser->arrayOptArgs[parser->countOptArgs].longName, longName, MAX_LEN_STR);
		}
	}
		
	if ( strDescription == NULL )
		parser->arrayOptArgs[parser->countOptArgs].strDescription[0] = '\0';
	else
		strncpy(parser->arrayOptArgs[parser->countOptArgs].strDescription, strDescription, MAX_LEN_STR);
	
	if ( parser->arrayGroups[idGroup].bMutuallyExclusive )
		parser->arrayOptArgs[parser->countOptArgs].bRequired = false;
	else
		parser->arrayOptArgs[parser->countOptArgs].bRequired = bRequired;
		
	parser->arrayOptArgs[parser->countOptArgs].eMob = eMob;

	if ( nArgsMax < 0 )
		nArgsMax = OR_MORE;
	
	if ( nArgsMin < 0 )
		nArgsMin = 0;
		
	if ( nArgsMax >= 0 && nArgsMax < nArgsMin )
		nArgsMax = nArgsMin;
		
	parser->arrayOptArgs[parser->countOptArgs].nArgsMin = nArgsMin;	
	parser->arrayOptArgs[parser->countOptArgs].nArgsMax = nArgsMax;
		
	parser->arrayOptArgs[parser->countOptArgs].countArgs = 0;
		
	if ( strTypes == NULL || strlen(strTypes) == 0 )
		parser->arrayOptArgs[parser->countOptArgs].strTypes[0] = '\0';
	else
	{
		strncpy(str, strTypes, MAX_LEN_STR);
		myopt_NormalizeStringTypes(str);
		strncpy(parser->arrayOptArgs[parser->countOptArgs].strTypes, str, MAX_LEN_STR);
	}
	
	if ( !myopt_CheckTypesString(parser, strOptionName, parser->arrayOptArgs[parser->countOptArgs].strTypes) )
		return false;
				
	parser->countOptArgs++;
	
	return true;
}

int myopt_LookupShort(myopt_Parser_t parser, char shortName)
{
	int x;
	
	for (x = 0; x < parser->countOptArgs; x++)
	{		
		if ( parser->arrayOptArgs[x].shortName == shortName )
			return x;
	}
	
	return -1;
}

int myopt_LookupLongPrefix(myopt_Parser_t parser, const char *longName)
{
	int x, y, z;
	int count = 0;
		
	z = -1;
	for (x = 0; x < parser->countOptArgs; x++)
	{
		y = 0;
		while ( longName[y] != '\0' )
		{
			if ( parser->arrayOptArgs[x].longName[y] == '\0' )
				return -1;
				
			if ( parser->arrayOptArgs[x].longName[y] != longName[y] )
				break;
			y++;
		}
		
		if ( longName[y] == '\0' )
		{
			count++;
			z = x;
		}
	}
	
	if ( count == 1 )
		return z;
	
	return -1;	
}

int myopt_LookupLong(myopt_Parser_t parser, const char *longName)
{
	int x;
		
	for (x = 0; x < parser->countOptArgs; x++)
	{
		if ( strcmp(parser->arrayOptArgs[x].longName, longName) == 0 )
			return x;
	}
	
	return myopt_LookupLongPrefix(parser, longName);
}

int myopt_FindLong(myopt_Parser_t parser, const char *longName)
{
	int x;
		
	for (x = 0; x < parser->countOptArgs; x++)
	{
		if ( strcmp(parser->arrayOptArgs[x].longName, longName) == 0 )
			return x;
	}
	
	return -1;
}


bool myopt_SetPositionalArgsParams(myopt_Parser_t parser, const char *strName, int32_t nArgsMin, int32_t nArgsMax, const char *strTypes)
{
	char str[MAX_LEN_STR];
	
	if ( strName == NULL || strlen(strName) == 0 )
		strncpy(parser->strPosArgsName, "POS_ARG", MAX_LEN_STR);
	else
		strncpy(parser->strPosArgsName, strName, MAX_LEN_STR);
		
	if ( nArgsMin < 0 )
		nArgsMin = 0;
	if ( nArgsMax < 0 )
		nArgsMax = OR_MORE;
	if ( nArgsMax >= 0 && nArgsMax < nArgsMin )
		nArgsMax = nArgsMin;		
	parser->nPosArgsMin = nArgsMin;
	parser->nPosArgsMax = nArgsMax;
		
	if ( strTypes == NULL || strlen(strTypes) == 0 )
		parser->strPosTypes[0] = '\0';
	else
	{
		strncpy(str, strTypes, MAX_LEN_STR);
		myopt_NormalizeStringTypes(str);
		strncpy(parser->strPosTypes, str, MAX_LEN_STR);
	}
	
	if ( !myopt_CheckTypesString(parser, parser->strPosArgsName, parser->strPosTypes) )
		return false;	
		
	return true;
}

void myopt_NormalizeStringTypes(char *strTypes)
{
	int len = strlen(strTypes);
	int x = 0;
	int i = 0;
	char strOut[MAX_LEN_STR];
	
	if ( strTypes == NULL )
		return;
	
	while (	x < len )
	{
		if ( strTypes[x] != ' ' && strTypes[x] != '\t' && strTypes[x] != '\n' )
		{
			strOut[i] = strTypes[x];		
			if ( strOut[i] >= 'A' && strOut[i] <= 'Z' )
				strOut[i] += 32;
			i++;
		}
		
		x++;
	}
	strOut[i] = '\0';
	
	strncpy(strTypes, strOut, MAX_LEN_STR);
}

bool myopt_CheckTypesString(myopt_Parser_t parser, const char *strOptionName, const char *strTypes)
{
	char str[MAX_LEN_STR];
	int x;
	int i;
	char strError[1024];
	int len;
	bool ret = true;
		
	len = strlen(strTypes);
	if ( len == 0 )
		return true;
	
	x = 0;
ciclo:
	i = 0;
	while ( x < len )
	{
		if ( strTypes[x] != ',' )
		{
			str[i++] = strTypes[x++];
		}
		else
		{
			x++;
			break;
		}
	}
	str[i] = '\0';
		
	if ( i > 0 )
	{		
		if ( strcmp(str, "int") == 0 )
			ret = true;		
		else if ( strcmp(str, "float") == 0 )
			ret = true;
		else if ( strcmp(str, "string") == 0 )
			ret = true;
		else
		{
			sprintf(strError, gettext("'%s': wrong type spec: '%s' in strTypes '%s'\n"), strOptionName, str, strTypes);
			strncat(parser->strInternalErrors, strError, STR_ERRORS_SIZE);
			parser->countInternalErrors++;
			return false;
		}		
	}
		
	if ( x < len )
		goto ciclo;

	return ret;
}

bool myopt_MakeOptionName(char shortName, const char *longName, char *strOptionName)
{
	bool bShort = false;
	strOptionName[0] = '\0';
	
	if ( (shortName == 0 || shortName == '\0') && (longName == NULL || strlen(longName) == 0) )
		return false;
	
	if ( shortName != 0 && shortName != '\0' )
	{
		strOptionName[0] = '-';
		strOptionName[1] = shortName;
		strOptionName[2] = '\0';
		bShort = true;
	}
	
	if ( longName != NULL && strlen(longName) > 0 )
	{
		if ( bShort )
			strncat(strOptionName, ", --", STR_ERRORS_SIZE);
		else
			strncat(strOptionName, "--", STR_ERRORS_SIZE);
		strncat(strOptionName, longName, MAX_LEN_STR);
	}	
	
	return true;
}

uint32_t myopt_MakeArgumentOptionHelp(myopt_Parser_t parser, uint32_t index, const char *strOptionName, char *strHelp)
{
	char strTemp[1024];		
	strHelp[0] = '\0';
	
	if ( !(parser->arrayOptArgs[index].nArgsMin == 0 && parser->arrayOptArgs[index].nArgsMax == 0) )
	{
		strncat(strHelp, gettext("   Arguments for option '"), STR_ERRORS_SIZE);
		strncat(strHelp, strOptionName, STR_ERRORS_SIZE);
		strncat(strHelp, "': ", STR_ERRORS_SIZE);
		
		if ( parser->arrayOptArgs[index].nArgsMax == OR_MORE )
		{
			if ( parser->arrayOptArgs[index].nArgsMin == 0 )
			{
				strncat(strHelp, gettext("zero or more"), STR_ERRORS_SIZE);
			}
			else if ( parser->arrayOptArgs[index].nArgsMin == 1 )
			{
				strncat(strHelp, gettext("one or more"), STR_ERRORS_SIZE);
			}
			else
			{
				sprintf(strTemp, gettext("%d or more"), parser->arrayOptArgs[index].nArgsMin);
				strncat(strHelp, strTemp, STR_ERRORS_SIZE);
			}		
		}
		else
		{
			if ( parser->arrayOptArgs[index].nArgsMin != parser->arrayOptArgs[index].nArgsMax )			
				sprintf(strTemp, gettext("between %d and %d"), parser->arrayOptArgs[index].nArgsMin, parser->arrayOptArgs[index].nArgsMax);
			else
				sprintf(strTemp, gettext("exactly %d"), parser->arrayOptArgs[index].nArgsMin);
			strncat(strHelp, strTemp, STR_ERRORS_SIZE);
		}
		
		strncat(strHelp, "(", STR_ERRORS_SIZE);
		strncat(strHelp, parser->arrayOptArgs[index].strTypes, STR_ERRORS_SIZE);
		strncat(strHelp, ")", STR_ERRORS_SIZE);
	}
	
	return strlen(strHelp);
}

void myopt_MakeUsageString(myopt_Parser_t parser)
{
	int x, y;
	char strOptionName[1024];
	char strTemp[1024];	
	int bGroupFree;
	
	if ( !parser )
		return;
		
	strncat(parser->strUsage, gettext("Usage:\n"), STR_ERRORS_SIZE);
	strncat(parser->strUsage, parser->strExeName, STR_ERRORS_SIZE);
	
	if ( parser->countOptArgs > 0 )
		strncat(parser->strUsage, gettext(" Options"), STR_ERRORS_SIZE);
	
	if ( !(parser->nPosArgsMin == 0 && parser->nPosArgsMax == 0) )
	{
		strncat(parser->strUsage, " ", STR_ERRORS_SIZE);
		strncat(parser->strUsage, parser->strPosArgsName, STR_ERRORS_SIZE);
		
		if ( parser->nPosArgsMax == OR_MORE )
		{
			if ( parser->nPosArgsMin == 0 )
			{
				strncat(parser->strUsage, gettext("(zero or more)"), STR_ERRORS_SIZE);
			}
			else if ( parser->nPosArgsMin == 1 )
			{
				strncat(parser->strUsage, gettext("(one or more)"), STR_ERRORS_SIZE);
			}
			else
			{
				sprintf(strTemp, gettext("(%d or more)"), parser->nPosArgsMin);
				strncat(parser->strUsage, strTemp, STR_ERRORS_SIZE);
			}		
		}
		else
		{
			if ( parser->nPosArgsMin != parser->nPosArgsMax )
				sprintf(strTemp, gettext("(between %d and %d)"), parser->nPosArgsMin, parser->nPosArgsMax);
			else
				sprintf(strTemp, gettext("(exactly %d)"), parser->nPosArgsMin);
			
			strncat(parser->strUsage, strTemp, STR_ERRORS_SIZE);			
		}
		
		strncat(parser->strUsage, "(", STR_ERRORS_SIZE);
		strncat(parser->strUsage, parser->strPosTypes, STR_ERRORS_SIZE);
		strncat(parser->strUsage, ")", STR_ERRORS_SIZE);
		strncat(parser->strUsage, "\n", STR_ERRORS_SIZE);
	}

	if ( parser->countOptArgs > 0 )
		strncat(parser->strUsage, gettext("\nOptions:\n"), STR_ERRORS_SIZE);
	
	for ( x = 0; x < parser->countGroups; x++ )
	{
		bGroupFree = 1;
		
		if ( parser->countGroups > 1 )
		{
			sprintf(strTemp, gettext("Group[%d]: %s:\n"), x, parser->arrayGroups[x].strDescription);
			strncat(parser->strUsage, strTemp, STR_ERRORS_SIZE);
		}
			
		if ( parser->arrayGroups[x].bMutuallyExclusive && parser->arrayGroups[x].bRequired )
		{
			bGroupFree = 0;
			strncat(parser->strUsage, gettext("Only one of:\n"), STR_ERRORS_SIZE);
		}
		
		if ( parser->arrayGroups[x].bRequired && !parser->arrayGroups[x].bMutuallyExclusive )
		{
			strncat(parser->strUsage, gettext("At least one of:\n"), STR_ERRORS_SIZE);
		}
			
		for ( y = 0; y < parser->countOptArgs; y++ )
		{	
			if ( parser->arrayOptArgs[y].idGroup == x )
			{
				myopt_MakeOptionName(parser->arrayOptArgs[y].shortName, parser->arrayOptArgs[y].longName, strOptionName);
				strncat(parser->strUsage, "'", STR_ERRORS_SIZE);
				strncat(parser->strUsage, strOptionName, STR_ERRORS_SIZE);
				strncat(parser->strUsage, "'", STR_ERRORS_SIZE);				
				if ( bGroupFree )
				{
					if ( parser->arrayOptArgs[y].bRequired )
						strncat(parser->strUsage, gettext(" (required)"), STR_ERRORS_SIZE);
					else
						strncat(parser->strUsage, gettext(" (optional)"), STR_ERRORS_SIZE);
				}
				strncat(parser->strUsage, "\t", STR_ERRORS_SIZE);
				strncat(parser->strUsage, parser->arrayOptArgs[y].strDescription, STR_ERRORS_SIZE);
				strncat(parser->strUsage, "\n", STR_ERRORS_SIZE);
				
				if ( myopt_MakeArgumentOptionHelp(parser, y, strOptionName, strTemp) > 0 )
				{
					strncat(parser->strUsage, strTemp, STR_ERRORS_SIZE);
					strncat(parser->strUsage, "\n", STR_ERRORS_SIZE);
				}
			}
		}
		strncat(parser->strUsage, "\n", STR_ERRORS_SIZE);
	}	
}

/* ---------------------------- Argument List ------------------------------------- */

myopt_ArgsList* myopt_ArgsListNewNode(myopt_ArgsList *Elem)
{
	myopt_ArgsList *n;

	n = (myopt_ArgsList*)malloc(sizeof(myopt_ArgsList));

	if( n == NULL )
		return NULL;

	n->arg.Type = Elem->arg.Type;
	if ( Elem->arg.strValue == NULL )
		n->arg.strValue[0] = '\0';
	else
		strncpy(n->arg.strValue, Elem->arg.strValue, MAX_LEN_STR);
	n->arg.intValue = Elem->arg.intValue;
	n->arg.floatValue = Elem->arg.floatValue;
	n->next = NULL;
	
	return n;
}

myopt_ArgsList* myopt_ArgsListAppend(myopt_ArgsList *Elem, myopt_ArgsList *first)
{
	myopt_ArgsList *n = first, *newnode;

	if ( first == NULL )
		return myopt_ArgsListNewNode(Elem);

	n = first;
	while( n->next != NULL )
	{
		n = n->next;
	}

	newnode = myopt_ArgsListNewNode(Elem);
	n->next = newnode;
	
	return first;
}

void myopt_ArgsListFree(myopt_ArgsList *first)
{
	myopt_ArgsList *n1 = first, *n2;
	
	while ( n1 != NULL )
	{
		n2 = n1->next;
		free(n1);
		n1 = n2;
	}
	first = NULL;
}
