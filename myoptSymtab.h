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

#ifndef __MY_OPT_SYMTAB_H
#define __MY_OPT_SYMTAB_H

#include "common.h"

BEGIN_C_DECLS

typedef enum _myopt_ArgumentTypeEnum
{	
	T_STRING,	
	T_INT,
	T_FLOAT
} myopt_ArgumentTypeEnum;

typedef enum _myopt_MOBEnum /* MOB = Multiple Occurrences Behavior */
{	
	MOB_ERROR,
	MOB_OVERRIDE,
	MOB_APPEND
} myopt_MOBEnum;

typedef struct _myopt_Argument
{
	myopt_ArgumentTypeEnum Type;
	char     strValue[MAX_LEN_STR + 1];
	int64_t  intValue;
	double   floatValue;
} myopt_Argument;

typedef struct _myopt_ArgsList
{
	myopt_Argument arg;
	struct _myopt_ArgsList *next;
} myopt_ArgsList;

typedef struct _myopt_Option
{
	int32_t idGroup;
	char shortName;
	char longName[MAX_LEN_STR + 1];
	char strDescription[MAX_LEN_STR + 1];
	bool bRequired;
	myopt_MOBEnum eMob;
	int32_t nArgsMin;
	int32_t nArgsMax; /* OR_MORE */	
	char strTypes[MAX_LEN_STR + 1];
	myopt_Argument *arrayArgs;
	int32_t countArgs;
	int32_t countOccurrences;
	/* Reserved: */
	myopt_ArgsList *listArgs;
} myopt_Option;

typedef struct _myopt_Group
{
	char strDescription[MAX_LEN_STR + 1];
	bool bRequired;	
	bool bMutuallyExclusive;
} myopt_Group;

typedef struct _myopt_Parser_t
{
	char *strErrors;
	char *strInternalErrors;
	char *strUsage;
	myopt_Option *arrayOptArgs;
	int32_t countOptArgs;
	myopt_Argument *arrayPosArgs;
	int32_t countPosArgs;
	myopt_Group *arrayGroups;
	int32_t countGroups;
	int32_t countInternalErrors;
	/* Reserved: */
	char *strInput;
	char strExeName[MAX_LEN_STR + 1];	
	char strPosArgsName[MAX_LEN_STR + 1];
	int32_t nPosArgsMin;
	int32_t nPosArgsMax; /* OR_MORE */
	char strPosTypes[MAX_LEN_STR + 1];
} *myopt_Parser_t;

myopt_Parser_t myopt_InitParser();
void myopt_FreeParser(myopt_Parser_t parser);

int32_t myopt_AddGroup(myopt_Parser_t parser, const char *strDescription, bool bMutuallyExclusive, bool bRequired);

bool myopt_AddOption(myopt_Parser_t parser,
                     int32_t idGroup,
                     char shortName,
                     const char *longName,
                     const char *strDescription,
                     bool bRequired,
                     myopt_MOBEnum eMob,
                     int32_t nArgsMin,
                     int32_t nArgsMax,                     
                     const char *strTypes);

int myopt_LookupShort(myopt_Parser_t parser, char shortName);
int myopt_LookupLong(myopt_Parser_t parser, const char *longName);
int myopt_LookupLongPrefix(myopt_Parser_t parser, const char *longName);

bool myopt_MakeOptionName(char shortName, const char *longName, char *strOptionName);
uint32_t myopt_MakeArgumentOptionHelp(myopt_Parser_t parser, uint32_t index, const char *strOptionName, char *strHelp);
void myopt_MakeUsageString(myopt_Parser_t parser);

void myopt_NormalizeStringTypes(char *strTypes);
bool myopt_CheckTypesString(myopt_Parser_t parser, const char *strOptionName, const char *strTypes);
bool myopt_SetPositionalArgsParams(myopt_Parser_t parser, const char *strName, int32_t nArgsMin, int32_t nArgsMax, const char *strTypes);

myopt_ArgsList* myopt_ArgsListNewNode(myopt_ArgsList *Elem);
myopt_ArgsList* myopt_ArgsListAppend(myopt_ArgsList *Elem, myopt_ArgsList *first);
void myopt_ArgsListFree(myopt_ArgsList *first);

END_C_DECLS

#endif /* __MY_OPT_SYMTAB_H */
