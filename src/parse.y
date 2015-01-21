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

	Name:     parse.y
	Revision: 1.4
	Date:     2015-01-19

PROGRAM INFORMATION

	Developed by:	CFI project
	Developer:	Douglas Jerome, drj, <douglas@ttylinux.org>

FILE DESCRIPTION

	Configuration File Interface: Parser

	This file implements the CFI parser's functions.

CHANGE LOG

	23jul05	drj	Gave this code another whipping.  Simplified the code
			and removed usage of libcsc.  Changed the name of the
			file from "cfi_parse.y" to "parse.y".

	07jul01	drj	Changed code to use new libcsc.

	24jun00	drj	Added fileName argument to function CFIparseText().

	23may00	drj	Added the action functions to this file. Changed
			cfidbLoad back to cfiParse.

	19aug99	drj	Changed cfiParse to cfidbLoad.

	30apr99	drj	Changed from using librt to using libcsc.

	02mar99	drj	File generation.

***************************************************************************** */


 /* ######################################################################## */
 /*                                                                          */
 /* YACC DEFINITION SECTION                                                  */
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
#include	"parse.h"


/* ************************************************************************* */
/*                                                                           */
/*      M a n i f e s t   C o n s t a n t s                                  */
/*                                                                           */
/* ************************************************************************* */

#define   PDEBUG(x)   {x;actions_dump(#x);}


/* ************************************************************************* */
/*                                                                           */
/*      P r i v a t e   G l o b a l   V a r i a b l e s                      */
/*                                                                           */
/* ************************************************************************* */

static CFI_node_t g_node;


/*****************************************************************************
 * Private Function Prototypes
 *****************************************************************************/

static __inline__ void       actions_init (void);
static __inline__ CFI_node_t actions_done (void);
static __inline__ void       actions_dump (const char* const text);


%}


%union
   {
   CFI_node_t nptr;
   CFI_attr_t aptr;
   char*      cptr;
   double     real;
   int32_t    num;
   }

/*
 * Ok, this takes some explaining -- %token "KEYWORD" is sort of Not Used.
 * All of the other tokens are a return value from the lexical analyzer; but
 * "KEYWORD" is actually not returned from the lexical analyzer.
%token		KEYWORD
 */
%token	<cptr>	CFIYY_WORD CFIYY_STRING
%token	<real>	CFIYY_REALNUM
%token	<num>	CFIYY_HEXNUM CFIYY_DECNUM CFIYY_OCTNUM CFIYY_BINNUM

%type	<nptr>	dictionary
%type	<nptr>	object
%type	<aptr>	attribute
%type	<nptr>	word
%type	<nptr>	word_attribute
%type	<aptr>	attribute_list
%type	<nptr>	section
%type	<aptr>	param_option


%%


 /* ######################################################################## */
 /*                                                                          */
 /* YACC RULES SECTION                                                       */
 /*                                                                          */
 /* ######################################################################## */

/*
<----------------------------------------------------------- 132 Columns ---------------------------------------------------------->
 */

/*
 * Here is the BNF for the grammer:
 * -------------------------------
 *
 * <dictionary>     ::=  { <word> | <word-attribute> | <section> }
 * <word>           ::=  <attribute> ';'
 * <word-attribute> ::=  <word> '=' <attribute> { ',' <attribute> } ';'
 * <section>        ::=  <word> [ '(' <attribute> ')' ] '{' { <dictionary> } '}'
 * <attribute>      ::=  <word> | <number> | <string>
 */

dictionary:	/* empty */	{ PDEBUG($$=NULL)                             }
	|	object dictionary
				{ PDEBUG($$=_cfi_node_join($1,$2)) g_node=$$; }
	;

object:		word		{ PDEBUG($$=$1) }
	|	word_attribute	{ PDEBUG($$=$1) }
	|	section		{ PDEBUG($$=$1) }
	;

word:		CFIYY_WORD ';'	{ PDEBUG($$=_cfi_node_word_new($1)) }
	;

word_attribute:	CFIYY_WORD '=' attribute_list ';'
				{
				PDEBUG($$=_cfi_node_attribute_new($1,$3))
				}
	;

section:	CFIYY_WORD param_option '{' dictionary '}'
				{
				PDEBUG($$=_cfi_node_section_new($1,$2,$4))
				}
	;

param_option:	/* empty */	{ PDEBUG($$=NULL) }
	|	'(' attribute ')'
				{ PDEBUG($$=$2)   }
	;

attribute_list:	attribute	{ PDEBUG($$=$1) }
	|	attribute ',' attribute_list
				{
				PDEBUG($$=_cfi_attribute_join($1,$3))
				}
	;

attribute:	CFIYY_STRING	{ PDEBUG($$=_cfi_attribute_new($1,CFI_STRING_ATTRIBUTE)); free($1); }
	|	CFIYY_WORD	{ PDEBUG($$=_cfi_attribute_new($1,CFI_WORD_ATTRIBUTE)); free($1);   }
	|	CFIYY_REALNUM	{ PDEBUG($$=_cfi_attribute_new(&$1,CFI_REAL_ATTRIBUTE))             }
	|	CFIYY_HEXNUM	{ PDEBUG($$=_cfi_attribute_new(&$1,CFI_HEX_FORMAT))                 }
	|	CFIYY_DECNUM	{ PDEBUG($$=_cfi_attribute_new(&$1,CFI_DEC_FORMAT))                 }
	|	CFIYY_OCTNUM	{ PDEBUG($$=_cfi_attribute_new(&$1,CFI_OCT_FORMAT))                 }
	|	CFIYY_BINNUM	{ PDEBUG($$=_cfi_attribute_new(&$1,CFI_BIN_FORMAT))                 }
	;


%%


 /* ######################################################################## */
 /*                                                                          */
 /* YACC USER SUBROUTINE SECTION                                             */
 /*                                                                          */
 /* ######################################################################## */


/* ************************************************************************* */
/*                                                                           */
/*      E x e c u t a b l e   C o d e   (Locally Used Functions)             */
/*                                                                           */
/* ************************************************************************* */


/*****************************************************************************
 * Private Function actions_init
 *****************************************************************************/

static __inline__ void actions_init (void)
   {
   g_node = NULL;
   }


/*****************************************************************************
 * Private Function actions_done
 *****************************************************************************/

static __inline__ CFI_node_t actions_done (void)
   {
   CFI_node_t node;

   node   = g_node;
   g_node = NULL;

   return node;
   }


/*****************************************************************************
 * Private Function actions_dump
 *****************************************************************************/

static __inline__ void actions_dump (const char* const a_text)
   {
   if (CFI_debugGrammar)
      {
      printf ("<PARSE>action: \"%s\"\n", a_text);
      }
   }


/* ************************************************************************* */
/*                                                                           */
/*      E x e c u t a b l e   C o d e   (External Interface Functions)       */
/*                                                                           */
/* ************************************************************************* */


/*****************************************************************************
 * Public Function cfi_parse_file
 *****************************************************************************/

CFI_node_t (cfi_parse_file) (FILE* a_file)
   {
   CFI_node_t node;

   cfi_lex_init (a_file, NULL);
   actions_init();

   (void)yyparse();

   node = actions_done();
   cfi_lex_done();

   return node;
   }


/*****************************************************************************
 * Public Function cfi_parse_text
 *****************************************************************************/

CFI_node_t (cfi_parse_text) (const char* a_text)
   {
   CFI_node_t node;

   void* yy_scan_string (const char*);
   void  yy_delete_buffer (void*);
   void* bufstate;

   bufstate = (void*)yy_scan_string (a_text);
   node = cfi_parse_file (NULL);
   (void)yy_delete_buffer (bufstate);

   return node;
   }


/* end of file */
