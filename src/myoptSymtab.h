/*
   Copyright (C) 2013 Vincenzo Lo Cicero

   Author: Vincenzo Lo Cicero.
   e-mail: vincenzo.locicero@libero.it
      
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

#include "myoptParser.h"

int myopt_LookupLongPrefix(myopt_Parser_t parser, const char *longName);


uint32_t myopt_MakeArgumentOptionHelp(myopt_Parser_t parser, uint32_t index, const char *strOptionName, char *strHelp);
void myopt_MakeUsageString(myopt_Parser_t parser);

void myopt_NormalizeStringTypes(char *strTypes);
bool myopt_CheckTypesString(myopt_Parser_t parser, const char *strOptionName, const char *strTypes);

myopt_ArgsList* myopt_ArgsListNewNode(myopt_ArgsList *Elem);
myopt_ArgsList* myopt_ArgsListAppend(myopt_ArgsList *Elem, myopt_ArgsList *first);
void myopt_ArgsListFree(myopt_ArgsList *first);

#endif /* __MY_OPT_SYMTAB_H */
