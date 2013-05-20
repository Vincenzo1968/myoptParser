/*
   Copyright (C) 2013 Vincenzo Lo Cicero

   Author: Vincenzo Lo Cicero.
   e-mail: vincenzo.locicero@libero.it
    
   From an idea by Leonardo Pozzati.
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

#ifndef __MY_OPT_PARSER_H
#define __MY_OPT_PARSER_H

#include "myoptLexer.h"
#include "myoptSymtab.h"

typedef struct _myopt_ParserData
{
	myopt_Token m_Token;
	myopt_Parser_t m_Parser;
} myopt_ParserData;


int myopt_ParseArray(myopt_Parser_t parser, int argc, char* argv[]);

int myopt_Parse(myopt_Parser_t parser, const char *strInput);

int myopt_Match(myopt_TokenTypeEnum ExpectedToken, myopt_ParserData *pd);

int myopt_ArgList(myopt_ParserData *pd);

int myopt_GetNextType(const char *strTypes, int len, int *nextPos, int bOnlyFirst, int lastType);
int myopt_GetValueType(myopt_Argument *arg);
int myopt_CheckTypes(myopt_ParserData *pd, myopt_Argument arrayArgs[], int32_t countPosArgs, char *strTypes, const char *strOptName, int bOnlyFirst);

void myopt_ExtractFileName(const char *szPathName, char *szFileName);

#endif /* __MY_OPT_PARSER_H */