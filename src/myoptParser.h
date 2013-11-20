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

#ifndef __MY_OPT_PARSER_H
#define __MY_OPT_PARSER_H

#if HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef __cplusplus
#  define BEGIN_C_DECLS extern "C" {
#  define END_C_DECLS   }
#else /* !__cplusplus */
#  define BEGIN_C_DECLS
#  define END_C_DECLS
#endif /* __cplusplus */

#define MAX_OPTS 256
#define MAX_LEN_STR 1024
#define STR_ERRORS_SIZE 8192
#define OR_MORE -1

#define ERRMSG_INSUFFICIENT_MEMORY "Error: insufficient memory\n"
#define MSG_GROUP "Group %d"
#define MSG_TOKEN_TOO_LONG "Error: token too long: '%s'.\nMax length is %d.\n"


#include <stdio.h>
#include <stdlib.h>  
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <malloc.h>
#include <stdint.h>
#include <libintl.h>
#include <locale.h>


#ifdef HAVE_STDBOOL_H
# include <stdbool.h>
#else
# ifndef HAVE__BOOL
#  ifdef __cplusplus
    typedef bool _Bool;
#  else
#   define _Bool signed char
#  endif
# endif
# define bool _Bool
# define false 0
# define true 1
# define __bool_true_false_are_defined 1
#endif

BEGIN_C_DECLS

typedef enum _myopt_TokenTypeEnum
{	
	T_SHORT,
	T_LONG,
	T_POSITIONAL,
	T_END_OPTIONS,
	T_EOL,
	T_ERROR
} myopt_TokenTypeEnum;

typedef struct _myopt_Token
{
	myopt_TokenTypeEnum Type;
	char str[MAX_LEN_STR + 1];
	int nNextPos;
	bool bEndOfOptions;
	bool bShortOptionState;
} myopt_Token;

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

typedef struct _myopt_ParserData
{
	myopt_Token m_Token;
	myopt_Parser_t m_Parser;
} myopt_ParserData;

/* ------- Functions defined in myoptSymtab.c ---------------- */

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
int myopt_FindLong(myopt_Parser_t parser, const char *longName);
bool myopt_MakeOptionName(char shortName, const char *longName, char *strOptionName);
bool myopt_SetPositionalArgsParams(myopt_Parser_t parser, const char *strName, int32_t nArgsMin, int32_t nArgsMax, const char *strTypes);

/* ------- Functions defined in myoptParser.c ---------------- */

int myopt_ParseArray(myopt_Parser_t parser, int argc, char* argv[]);
int myopt_Parse(myopt_Parser_t parser, const char *strInput);

END_C_DECLS

#endif /* __MY_OPT_PARSER_H */
