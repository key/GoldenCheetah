%{
/*
 * Copyright (c) 2010 Mark Liversedge (liversedge@gmail.com)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "WithingsParser.h"
#include <QDebug>

// we use stdio for reading from FILE *WithingsParserin
// because thats what lex likes to do, and since we're
// reading files that seems ok anyway
#include <stdio.h>

// The parser defines the token values for us
// so lets include them before declaring the
// token patterns
#include "WithingsParser_yacc.h"/* generated by the scanner */

// the options below tell flex to no bother with
// yywrap since we only ever read a single file
// anyway. And yyunput() isn't needed for our
// parser, we read in one pass with no swanky
// interactions

%}
%option noyywrap
%option nounput
%option noinput
%%
\"status\"            return STATUS;
\"body\"              return BODY;
\"updatetime\"        return UPDATETIME;
\"measuregrps\"       return MEASUREGRPS;
\"grpid\"             return GRPID;
\"attrib\"            return ATTRIB;
\"date\"              return DATE;
\"category\"          return CATEGORY;
\"comment\"           return COMMENT;
\"measures\"          return MEASURES;
\"value\"             return VALUE;
\"type\"              return TYPE;
\"unit\"              return UNIT;
[-+]?[0-9]+           return INTEGER;
[-+]?[0-9]+e-[0-9]+   return FLOAT;
[-+]?[0-9]+\.[-e0-9]* return FLOAT;
\"([^\"]|\\\")*\"     return STRING;  /* contains non-quotes or escaped-quotes */
[ \n\t\r]             ;               /* we just ignore whitespace */
.                     return WithingsParsertext[0]; /* any other character, typically :, { or } */
%%

static YY_BUFFER_STATE my_state;

/* set input to supplied string */
void WithingsParser_set(QString text)
{
    my_state = WithingsParser_scan_string(text.toLatin1().constData());
}

/* reset input */
void WithingsParser_reset()
{
    WithingsParser_delete_buffer (my_state);
}