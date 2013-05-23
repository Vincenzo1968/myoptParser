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

#ifndef __MY_OPT_LEXER_H
#define __MY_OPT_LEXER_H

#include "common.h"
#include "myoptSymtab.h"

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
	char str[MAX_LEN_STR];
	int nNextPos;
	bool bEndOfOptions;
	bool bShortOptionState;
} myopt_Token;

void myopt_InitToken(myopt_Token *tok);
int myopt_IsSeparator(int c);
myopt_TokenTypeEnum myopt_GetNextToken(myopt_Parser_t parser, myopt_Token *token);

END_C_DECLS

#endif /* __MY_OPT_LEXER_H */
