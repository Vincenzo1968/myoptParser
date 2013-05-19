/*
   Copyright (C) 2013 Vincenzo Lo Cicero

   Author: Vincenzo Lo Cicero.
   e-mail: vincenzo.locicero@libero.it
    
   From an idea by Leonardo Pozzati.

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
#include <ctype.h>

#include "myoptLexer.h"
//#include "myoptParser.h"

void myopt_InitToken(myopt_Token *tok)
{
	tok->str[0] = '\0';
	tok->nNextPos = 0;	
	tok->bEndOfOptions = false;
	tok->bShortOptionState = false;
}

int myopt_IsSeparator(int c)
{
	return ( c == ' ' || c == '\t' || c == '\n' );
}

myopt_TokenTypeEnum myopt_GetNextToken(myopt_Parser_t parser, myopt_Token *token)
{
	int i = 0;
	int x = -1;
	char strError[1024];
	char c;

	while ( 1 )
	{
		if ( parser->strInput[token->nNextPos] == '"' )
			token->bShortOptionState = false;
			
		if ( myopt_IsSeparator(parser->strInput[token->nNextPos]) )
		{
			token->bShortOptionState = false;
			
			while ( myopt_IsSeparator(parser->strInput[token->nNextPos++]) )
				;
			--token->nNextPos;					
		}
		
		if ( parser->strInput[token->nNextPos] == '-' && !token->bEndOfOptions )
		{
			i = 0;
			token->str[i++] = '-';
			token->nNextPos++;
			
			if ( myopt_IsSeparator(parser->strInput[token->nNextPos]) || parser->strInput[token->nNextPos] == '\0' )
			{
				token->Type = T_ERROR;				
				strcat(parser->strErrors, "Error: wrong option '-'\n");
				token->nNextPos++;
				return token->Type;
			}
			
			if ( isdigit(parser->strInput[token->nNextPos]) )
			{
				x = myopt_LookupShort(parser, parser->strInput[token->nNextPos]);
				if ( x < 0 )
				{
					c = parser->strInput[token->nNextPos++];
					while ( !isspace(c) && c != '\0'  )
					{
						token->str[i++] = c;
						c = parser->strInput[token->nNextPos++];
					}
					token->str[i] = '\0';
					token->Type = T_POSITIONAL;
					token->bShortOptionState = false;
					return token->Type;
				}				
			}			
			
			if ( parser->strInput[token->nNextPos] == '-' )
			{
				if ( myopt_IsSeparator(parser->strInput[token->nNextPos + 1]) || parser->strInput[token->nNextPos + 1] == '\0' )
				{
					token->str[i++] = '-';
					token->str[i] = '\0';
					token->Type = T_END_OPTIONS;
					token->bEndOfOptions = true;
					token->bShortOptionState = false;
					token->nNextPos++;
					return token->Type;
				}
									
				token->Type = T_LONG;
				
				while ( !myopt_IsSeparator(parser->strInput[token->nNextPos]) && parser->strInput[token->nNextPos + 1] != '\0' )
				{
					if ( parser->strInput[token->nNextPos] == '=' )
					{
						if ( myopt_IsSeparator(parser->strInput[token->nNextPos + 1]) )
						{
							token->Type = T_ERROR;							
							token->str[i] = '\0';
							sprintf(strError, "Error: option '%s': there must be no spaces before and after the equal sign\n", token->str);
							strcat(parser->strErrors, strError);
							token->nNextPos++;
							return token->Type;
						}
						token->nNextPos++;
						break;
					}
					token->str[i++] = parser->strInput[token->nNextPos++];
				}				
			}
			else
			{
				token->Type = T_SHORT;
				token->bShortOptionState = true;									
				token->str[i++] = parser->strInput[token->nNextPos++];

				x = myopt_LookupShort(parser, token->str[1]);
				if ( x < 0 )
				{
					token->str[i] = '\0';										
					token->Type = T_ERROR;
					sprintf(strError, "Error: unrecognized option '%s'\n", token->str);
					strcat(parser->strErrors, strError);
					return token->Type;
				}				
				if ( parser->arrayOptArgs[x].nArgsMin > 0 )
					token->bShortOptionState = false;
			}		
			
			token->str[i] = '\0';
			
			if ( token->Type == T_LONG )
			{
				x = myopt_LookupLong(parser, (token->str + 2));
				if ( x < 0 )
				{
					token->Type = T_ERROR;					
					sprintf(strError, "Error: unrecognized option '%s'\n", token->str);
					strcat(parser->strErrors, strError);
					return token->Type;
				}
			}
			
			return token->Type;
		}
		else if ( parser->strInput[token->nNextPos] == '\0' )
		{
			token->Type = T_EOL;
			token->str[0] = '$';
			token->str[1] = '\0';
			token->nNextPos = 0;
			return T_EOL;
		}
		else if ( parser->strInput[token->nNextPos] == '"' )
		{
			token->Type = T_POSITIONAL;
			token->nNextPos++;
			while ( parser->strInput[token->nNextPos] != '"' )
			{
				token->str[i++] = parser->strInput[token->nNextPos++];
			}					
			token->str[i] = '\0';
			token->nNextPos++;
			return token->Type;						
		}
		else
		{
			if ( !token->bShortOptionState )
			{
				token->Type = T_POSITIONAL;
				while ( !myopt_IsSeparator(parser->strInput[token->nNextPos]) && parser->strInput[token->nNextPos + 1] != '\0' )
				{
					token->str[i++] = parser->strInput[token->nNextPos++];
				}				
			}
			else
			{
				token->Type = T_SHORT;
				x = myopt_LookupShort(parser, parser->strInput[token->nNextPos]);
				if ( x < 0 )
				{						
					token->Type = T_ERROR;					
					sprintf(strError, "Error: unrecognized option '-%c'\n", parser->strInput[token->nNextPos]);
					strcat(parser->strErrors, strError);
					token->nNextPos++;
					return token->Type;
				}
				if ( parser->arrayOptArgs[x].nArgsMin > 0 )
					token->bShortOptionState = false;
				
				token->str[i++] = '-';
				token->str[i++] = parser->strInput[token->nNextPos++];
			}
			
			token->str[i] = '\0';
			return token->Type;			
		}			
	}

	return T_EOL;
}
