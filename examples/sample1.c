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

/* http://forum.ubuntu-it.org/viewtopic.php?f=33&t=556461 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "myoptParser.h"

/*
gcc -Wall -W -O2 -std=c99 sample1.c myoptParser.c myoptLexer.c myoptSymtab.c -o sample1

OR

gcc -c -Wall -W -O2 -std=c99 myoptParser.c myoptLexer.c myoptSymtab.c
ar  -r -s libmyoptParser.a myoptParser.o myoptLexer.o myoptSymtab.o
gcc -Wall -W -O2 -std=c99 -L. sample1.c -lmyoptParser -o sample1

*/

int main(int argc, char* argv[])
{
	myopt_Parser_t parser;
	int x, y;
	char strOptionName[1024];
	char strDescr[MAX_LEN_STR + 1];
	int32_t idGroup;
	int32_t idOption;
	bool bParse;
		
	parser = myopt_InitParser();
	if ( !parser )
		return -1;
		
	/* myopt_SetLang(parser, LANG_ENGLISH); */
	/* myopt_SetLang(parser, LANG_ITALIAN); */

	idGroup = myopt_AddGroup(parser, "Gruppo 0", false, false);
	
	strcpy(strDescr, "Print this message and exit");
	myopt_AddOption(parser, idGroup, 'h', "help", strDescr, false, MOB_OVERRIDE, 0, 0, "");	

	strcpy(strDescr, "Description option -x");
	myopt_AddOption(parser, idGroup, 'x', "xlong-prova", strDescr, true, MOB_OVERRIDE, 0, 0, "");
	
	strcpy(strDescr, "Description option -y");	
	myopt_AddOption(parser, idGroup, 'y', "ylong", strDescr, false, MOB_OVERRIDE, 0, 0, "");
	
	strcpy(strDescr, "Description option -z");	
	myopt_AddOption(parser, idGroup, 'z', "zlong", strDescr, false, MOB_ERROR, 0, 0, "");


	idGroup = myopt_AddGroup(parser, "Gruppo 1", false, true);

	strcpy(strDescr, "Description option -a");
	myopt_AddOption(parser, idGroup, 'a', "along", strDescr, false, MOB_OVERRIDE, 1, 1, "string");
	
	strcpy(strDescr, "Description option -b");	
	myopt_AddOption(parser, idGroup, 'b', "blong", strDescr, false, MOB_APPEND, 1, 1, "int");
	
	strcpy(strDescr, "Description option -c");	
	myopt_AddOption(parser, idGroup, 'c', "clong", strDescr, false, MOB_APPEND, 1, 1, "float");
	
	
	idGroup = myopt_AddGroup(parser, "Gruppo 2", true, true);
	
	strcpy(strDescr, "Description option -d");	
	myopt_AddOption(parser, idGroup, 'd', "dlong", strDescr, false, MOB_OVERRIDE, 1, 1, "string");
	
	strcpy(strDescr, "Description option -e");	
	myopt_AddOption(parser, idGroup, 'e', "elong", strDescr, false, MOB_ERROR, 0, OR_MORE, "string");
	
	strcpy(strDescr, "Description option -f");	
	myopt_AddOption(parser, idGroup, 'f', "flong", strDescr, false, MOB_ERROR, 1, OR_MORE, "string");
	
	strcpy(strDescr, "Description option -g");	
	myopt_AddOption(parser, idGroup, 'g', "glong", strDescr, false, MOB_ERROR, 3, OR_MORE, "string, int, float");
	
	myopt_SetPositionalArgsParams(parser, "POS_ARGS", 0, OR_MORE, "string, int, float, string, string");
		
	bParse = myopt_ParseArray(parser, argc, argv);
	
	if ( parser->countInternalErrors > 0 )
	{
		printf("Internal errors:\n");
		printf("%s\n", parser->strInternalErrors);
		return -1;
	}		
	
	//idOption = myopt_LookupShort(parser, 'h');
	idOption = myopt_LookupLong(parser, "help");
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
		
	for ( x = 0; x < parser->countOptArgs; x++ )
	{
		if ( parser->arrayOptArgs[x].countOccurrences > 0 )
		{
			myopt_MakeOptionName(parser->arrayOptArgs[x].shortName, parser->arrayOptArgs[x].longName, strOptionName);
			printf("Option '%s' %d occurrences\n", strOptionName, parser->arrayOptArgs[x].countOccurrences);
			if ( parser->arrayOptArgs[x].countArgs > 0 )
				printf("   %d arguments for option '%s':\n", parser->arrayOptArgs[x].countArgs, strOptionName);
			for ( y = 0; y < parser->arrayOptArgs[x].countArgs; y++ )
			{
				switch ( parser->arrayOptArgs[x].arrayArgs[y].Type )
				{
					case T_STRING:			
						printf("   string : %s", parser->arrayOptArgs[x].arrayArgs[y].strValue);
						break;
					case T_INT:
						printf("   int    : %lli", (long long int)parser->arrayOptArgs[x].arrayArgs[y].intValue);
						break;
					case T_FLOAT:
						printf("   float  : %g", parser->arrayOptArgs[x].arrayArgs[y].floatValue);
						break;
				}
				printf("\n");
			}			
		}			
	}
		
	if ( parser->countPosArgs )
		printf("\nList of positional argument:\n");
	for ( x = 0; x < parser->countPosArgs; x++ )
	{
		switch (parser->arrayPosArgs[x].Type )
		{
			case T_STRING:			
				printf("string : %s", parser->arrayPosArgs[x].strValue);
				break;
			case T_INT:
				printf("int    : %lli", (long long int)parser->arrayPosArgs[x].intValue);
				break;
			case T_FLOAT:
				printf("float  : %g", parser->arrayPosArgs[x].floatValue);
				break;
		}
		printf("\n");
	}
		
	myopt_FreeParser(parser);
	
	return 0;
}

