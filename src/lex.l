/*
 * This file is part of the CFI software.
 * The license which this software falls under is as follows:
 *
 * Copyright (C) 1999-2005 Douglas Jerome <douglas@ttylinux.org>
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or (at
 * your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


/* *****************************************************************************

FILE NAME

	Name:     lex.l
	Revision: 1.4
	Date:     2015-01-19

PROGRAM INFORMATION

	Developed by:	CFI project
	Developer:	Douglas Jerome, drj, <douglas@ttylinux.org>

FILE DESCRIPTION

	Configuration File Interface: Lexical Analyzer

	This file implements the CFI lexical analyzer's functions.

CHANGE LOG

	23jul05	drj	Gave this code another whipping.  Simplified the code
			and removed usage of libcsc.  Changed the name of the
			file from "cfi_lex.l" to "lex.l".

	07jul01	drj	Changed code to use new libcsc.

	23may00	drj	Gave this code a general whipping.  Added string
			interpretation from libPropList.

	26sep99	drj	Modified the error output to not report the proces ID
			when not compiled with the DEBUG c-switch.

	30apr99	drj	Changed from using librt to using libcsc.

	02mar99	drj	File generation.

***************************************************************************** */


 /* ######################################################################## */
 /*                                                                          */
 /* LEX DEFINITION SECTION                                                   */
 /*                                                                          */
 /* ######################################################################## */


%{


/* ************************************************************************* */
/*                                                                           */
/*      I n c l u d e d   F i l e s                                          */
/*                                                                           */
/* ************************************************************************* */

/*
 * OS Specific Header Files
 */
#ifdef	WIN32
#   include	"stdafx.h"
#endif

/*
 * Standard C (ANSI) Header Files
 */
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

/*
 * Posix Header Files
 */
#ifndef	WIN32
#   include	<unistd.h> /* always first amongst POSIX header files */
#endif

/*
 * Project Specific Header Files
 */
#include	"CFI.h"
#include	"lex.h"
#include	"y.tab.h"


/* ************************************************************************* */
/*                                                                           */
/*      M a n i f e s t   C o s t a n t s                                    */
/*                                                                           */
/* ************************************************************************* */

/*
 * Define YY_NO_INPUT; this tells flex to not generate the 'input' function
 * as that function is unused and makes a build-time warning.
 */
#define YY_NO_INPUT


/* ************************************************************************* */
/*                                                                           */
/*      P r i v a t e   G l o b a l   V a r i a b l e s                      */
/*                                                                           */
/* ************************************************************************* */

static CFI_yyerrorfn_t yyerrfn;

static int yyln;
static int yyOldState;
static int yyBlockComment;


/* ************************************************************************* */
/*                                                                           */
/*      E x e c u t a b l e   C o d e   (Locally Used Functions)             */
/*                                                                           */
/* ************************************************************************* */


/*****************************************************************************
 * Private Function Prototypes
 *****************************************************************************/

static int yywrap (void);
static __inline__ int yylval_make (int token);


/*****************************************************************************
 * Private Function yywrap
 *****************************************************************************/

static int yywrap (void)
   {
   return 1;
   }


/*****************************************************************************
 * Private Function yylval_make
 *****************************************************************************/

static __inline__ int yylval_make (int a_token)
   {
   char  buff[36];
   char* bufPtr;

   yylval.cptr = NULL;
   yylval.real = 0.0;
   yylval.num  = 0;

   switch (a_token)
      {

      default:  yylval.num = a_token;
                if (CFI_debugLexical)
                   {
                   printf ("<LEX symbol>: \"%c\"\n", (char)a_token);
                   }
                break;

      case CFIYY_STRING:
         {
         for (bufPtr=&yytext[0] ; *bufPtr != '\0' ; bufPtr++)
         if (*bufPtr == '\n') yyln++;
         yylval.cptr = (char*)calloc (1, yyleng);
         (void)memcpy (yylval.cptr, yytext, yyleng-1);
         if (CFI_debugLexical)
            {
            printf ("<LEX>CFIYY_STRING: \"%s\"\n", yylval.cptr);
            }
         }
         break;

      case CFIYY_WORD:
         {
         yylval.cptr = (char*)calloc (1, yyleng+1);
         (void)memcpy (yylval.cptr, yytext, yyleng);
         if (CFI_debugLexical)
            {
            printf ("<LEX>CFIYY_WORD: \"%s\"\n", yylval.cptr);
            }
         }
         break;

      case CFIYY_REALNUM:
         {
         yylval.real = atof (yytext);
         if (CFI_debugLexical)
            {
            printf ("<LEX>CFIYY_REALNUM: %+12.6E\n", yylval.real);
            }
         }
         break;

      case CFIYY_HEXNUM:
         {
         sscanf (yytext, "%x", &yylval.num);
         if (CFI_debugLexical)
            {
            printf ("<LEX>CFIYY_HEXNUM: 0x%08lx\n", (unsigned long)yylval.num);
            }
         }
         break;

      case CFIYY_DECNUM:
         {
         yylval.num = atoi (yytext);
         if (CFI_debugLexical)
            {
            printf ("<LEX>CFIYY_DECNUM: %ld\n", (long)yylval.num);
            }
         }
         break;

      case CFIYY_OCTNUM:
         {
         yylval.num = 0;
         for (bufPtr = &yytext[2] ; *bufPtr != '\0' ; bufPtr++)
            yylval.num = yylval.num * 8 + (*bufPtr - '0');
         if (CFI_debugLexical)
            {
            printf (
                   "<LEX>CFIYY_OCTNUM: %s\n",
                   cfi_string_octal (buff, yylval.num)
                   );
            }
         }
         break;

      case CFIYY_BINNUM:
         {
         yylval.num = 0;
         for (bufPtr = &yytext[2] ; *bufPtr != '\0' ; bufPtr++)
            yylval.num = yylval.num * 2 + (*bufPtr - '0');
         if (CFI_debugLexical)
            {
            printf (
                   "<LEX>CFIYY_BINNUM: %s\n",
                   cfi_string_binary (buff, yylval.num)
                   );
            }
         }
         break;

      }

   return a_token;
   }


%}


%x CODE COMMENT QUOTE

newline		\n
whitespace	[\t\f ]+
dash_comment	--.*$
hash_comment	#.*$
slash_comment	\/\/.*$
string		[^\"]*\"
word		[A-Za-z]((_[A-Za-z0-9])|([A-Za-z0-9]))*
exp_num		[-+]?[0-9]*\.[0-9]+([eE][-+]?[0-9]+)?
hex_num		0[xX][0-9A-Fa-f]+
num		[-+]?[0-9]+
oct_num		0[oO][0-7]+
bin_num		0[bB][0-1]+
symbol		[!@#$%^&*()_+|~\-=\\`{}[\]:";'<>?,./]
garbage		.


%option nounput


%%


 /* ######################################################################## */
 /*                                                                          */
 /* LEX RULES SECTION                                                        */
 /*                                                                          */
 /* ######################################################################## */


<QUOTE>{string}		{
			if (yyleng==1)
				{
				BEGIN yyOldState;
				return yylval_make(CFIYY_STRING);
				}
			if (yytext[yyleng-2] == '\\')
				{
				yymore();
				}
			else
				{
				BEGIN yyOldState;
				return yylval_make(CFIYY_STRING);
				}
			}

<COMMENT>\n		{ yyln++;                                           }
<COMMENT>.		;
<COMMENT>"/*"		{ yyBlockComment++;                                 }
<COMMENT>"*/"		{ if(--yyBlockComment==0) BEGIN yyOldState;         }

<CODE>{newline}		{ yyln++;                                           }
<CODE>"/*"		{ yyBlockComment=1; yyOldState=CODE; BEGIN COMMENT; }
<CODE>{whitespace}	{                                                   }
<CODE>{dash_comment}	{                                                   }
<CODE>{hash_comment}	{                                                   }
<CODE>{slash_comment}	{                                                   }
<CODE>\"		{ yyOldState=CODE; BEGIN QUOTE;                     }
<CODE>{word}		{ return yylval_make(CFIYY_WORD);                   }
<CODE>{exp_num}		{ return yylval_make(CFIYY_REALNUM);                }
<CODE>{hex_num}		{ return yylval_make(CFIYY_HEXNUM);                 }
<CODE>{num}		{ return yylval_make(CFIYY_DECNUM);                 }
<CODE>{oct_num}		{ return yylval_make(CFIYY_OCTNUM);                 }
<CODE>{bin_num}		{ return yylval_make(CFIYY_BINNUM);                 }
<CODE>{symbol}		{ return yylval_make(yytext[0]);                    }
<CODE>{garbage}		{
			if (CFI_debugLexical)
				{
				printf ("<Lexical TRASH -->");
				printf ("%s", yytext);
				printf ("<-- Lexical TRASH>");
				}
			}


%%


 /* ######################################################################## */
 /*                                                                          */
 /* LEX USER SUBROUTINE SECTION                                              */
 /*                                                                          */
 /* ######################################################################## */


/*****************************************************************************
 * Public Function yyerror
 *****************************************************************************/

void (yyerror) (char* s)
   {
   (*yyerrfn) (yyln, s, yytext);
   }


/*****************************************************************************
 * Public Function cfi_lex_init
 *****************************************************************************/

void (cfi_lex_init) (FILE* a_file, CFI_yyerrorfn_t a_errorfn)
   {
   yyerrfn = a_errorfn;
   yyln = 1;
   yyin = a_file;
   BEGIN CODE;
   }


/*****************************************************************************
 * Public Function cfi_lex_done
 *****************************************************************************/

void (cfi_lex_done) (void)
   {
   yyin = (FILE*)NULL;
   }


/* end of file */
