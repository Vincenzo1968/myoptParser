/*
   Copyright (C) 2013 Vincenzo Lo Cicero

   Author: Vincenzo Lo Cicero.
   e-mail: vincenzo.locicero@libero.it
    
   From an idea by Leonardo Pozzati <e-mail: leon.pozzati@gmail.com>.
   
   Special thanks to Francesco Baro <e-mail: francesco.baro@libero.it >
   for code review and bug fixing.
   
   This file use myoptParserLib.

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

/*
gcc -Wall -W -O2 -std=c99 sample1.c myoptParser.c myoptLexer.c myoptSymtab.c -o sample2

OR

gcc -c -Wall -W -O2 -std=c99 myoptParser.c myoptLexer.c myoptSymtab.c
ar  -r -s libmyoptParser.a myoptParser.o myoptLexer.o myoptSymtab.o
gcc -Wall -W -O2 -std=c99 -L. sample2.c -lmyoptParser -o sample2
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "myoptParser.h"

int main(int argc, char* argv[])
{
	myopt_Parser_t parser;
	int x;
	char strDescr[MAX_LEN_STR + 1];
	int32_t idGroup;
	int32_t idOption;
	double num;
	bool bParse;
		
	parser = myopt_InitParser();
	if ( !parser )
		return -1;

	idGroup = myopt_AddGroup(parser, "Gruppo 0", true, true);
	
	strcpy(strDescr, "Print this message and exit");
	myopt_AddOption(parser, idGroup, 'h', "help", strDescr, false, MOB_OVERRIDE, 0, 0, "");	

	strcpy(strDescr, "Accumulator: print sum of positionals arguments");
	myopt_AddOption(parser, idGroup, 'a', "accumulate", strDescr, true, MOB_OVERRIDE, 0, 0, "");
	
	strcpy(strDescr, "Max: print the maximum of the positional arguments");	
	myopt_AddOption(parser, idGroup, 'm', "max", strDescr, false, MOB_OVERRIDE, 0, 0, "");
		
	myopt_SetPositionalArgsParams(parser, "NUMS", 1, OR_MORE, "float");
		
	bParse = myopt_ParseArray(parser, argc, argv);
	
	if ( parser->countInternalErrors > 0 )
	{
		printf("Internal errors:\n");
		printf("%s\n", parser->strInternalErrors);
		return -1;
	}		
	
	idOption = myopt_LookupShort(parser, 'h');
	//idOption = myopt_LookupLong(parser, "help");
	if ( idOption >= 0 && parser->arrayOptArgs[idOption].countOccurrences > 0 )
	{
		printf("%s\n", parser->strUsage);
		myopt_FreeParser(parser);
		return 0;
	}	
	
	if ( !bParse )
	{
		printf("%s\n", parser->strUsage);
		printf("%s\n", parser->strErrors);
		myopt_FreeParser(parser);
		return -1;		
	}
	
	idOption = myopt_LookupShort(parser, 'a');
	//idOption = myopt_LookupLong(parser, "accumulate");
	if ( idOption >= 0 && parser->arrayOptArgs[idOption].countOccurrences > 0 )
	{
		num = 0;
		for ( x = 0; x < parser->countPosArgs; x++ )
			num += parser->arrayPosArgs[x].floatValue;
		printf("\nThe sum is: %f\n", num);
	}
	
	idOption = myopt_LookupShort(parser, 'm');
	//idOption = myopt_LookupLong(parser, "max");
	if ( idOption >= 0 && parser->arrayOptArgs[idOption].countOccurrences > 0 )
	{
		num = 0;
		if ( parser->countPosArgs > 0 )
			num = parser->arrayPosArgs[0].floatValue;			
		for ( x = 0; x < parser->countPosArgs; x++ )
			if ( parser->arrayPosArgs[x].floatValue > num )
				num = parser->arrayPosArgs[x].floatValue;
		printf("\nThe maximum is: %g\n", num);		
	}
		
	myopt_FreeParser(parser);
	
	return 0;
}
