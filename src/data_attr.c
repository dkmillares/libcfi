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

	Name:     data_attr.c
	Revision: 1.5
	Date:     2015-01-19

PROGRAM INFORMATION

	Developed by:	CFI project
	Developer:	Douglas Jerome, drj, <douglas@ttylinux.org>

FILE DESCRIPTION

	Configuration File Interface: Attribute Implementation

	This file contains the CFI functions that create, destroy, query, and
	otherwise directly manipulate CFI attributes; these are the attributes
	of CFI word-attribute pairs and parameters to CFI sections.

CHANGE LOG

	23jul05	drj	Culled these CFI attribute functions from a larger file
			that contained many different kinds of functions.
			Removed use of libcsc.

	03may02	drj	Fixed up function prototypes. Small comment changes.

	07jul01	drj	Changed code to use new libcsc.

	12mar00	drj	Added functions for getting to the attributes and their
			values.

	19may00	drj	Completely re-wrote this file.

	16sep99	drj	Added type argument to cfidbSearch; added new function
			cfidbSearchFlat.

	20aug99	drj	Changed attribute symbols to contain type designations
			that are in the exported header file, not the internal
			parser types.

	30apr99	drj	Changed from using librt to using libcsc. Moved
			constants to a header file and prefixed them with CFI_.

	28feb99	drj	File generation.

***************************************************************************** */


/* ************************************************************************* */
/*                                                                           */
/*      F e a t u r e   S w i t c h e s                                      */
/*                                                                           */
/* ************************************************************************* */

/*
 * Select these feature by moving them from the `if UNDEF' into the `else'
 * section.
 */
#ifdef	UNDEF
#   define	_BSD_SOURCE	1	/* 4.3+bsd subsystems           */
#   define	_POSIX_SOURCE	1	/* posix.1                      */
#   define	_POSIX_C_SOURCE	199309L	/* posix.1 and posix.4          */
#   define	_POSIX_C_SOURCE	199506L	/* posix.1 and posix.4 and MORE */
#else
#   define	_POSIX_C_SOURCE	200112L	/* posix.1 and posix.4 and MORE */
#   undef	_REENTRANT
#   define	_REENTRANT		/* thread-safe for glibc        */
#endif


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
#include	<stdlib.h>

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
#include	"symbol.h"


/* ************************************************************************* */
/*                                                                           */
/*      C o n s t a n t s                                                    */
/*                                                                           */
/* ************************************************************************* */

/* (none) */


/* ************************************************************************* */
/*                                                                           */
/*      E x t e r n a l   R e f e r e n c e s                                */
/*                                                                           */
/* ************************************************************************* */

/* (none) */


/* ************************************************************************* */
/*                                                                           */
/*      D a t a   T y p e s   a n d   S t r u c t u r e s                    */
/*                                                                           */
/* ************************************************************************* */

typedef struct S_attr_t
   {
   struct S_attr_t* next;
   S_sym_t*         symbol;
   }
   S_attr_t;


/* ************************************************************************* */
/*                                                                           */
/*      P u b l i c   G l o b a l   V a r i a b l e s                        */
/*                                                                           */
/* ************************************************************************* */

/* (none) */


/* ************************************************************************* */
/*                                                                           */
/*      P r i v a t e   G l o b a l   V a r i a b l e s                      */
/*                                                                           */
/* ************************************************************************* */

/* (none) */


/* ************************************************************************* */
/*                                                                           */
/*      E x e c u t a b l e   C o d e   (Locally Used Functions)             */
/*                                                                           */
/* ************************************************************************* */


/*****************************************************************************
 * Private Function Prototypes
 *****************************************************************************/

/* (none) */


/*****************************************************************************
 * Private Function
 *****************************************************************************/

/* (none) */


/* ************************************************************************* */
/*                                                                           */
/*      E x e c u t a b l e   C o d e   (External Interface Functions)       */
/*                                                                           */
/* ************************************************************************* */


/*****************************************************************************
 * Public Function _cfi_attribute_join
 *****************************************************************************/

CFI_attr_t (_cfi_attribute_join) (CFI_attr_t a_attr1, CFI_attr_t a_attr2)
   {
   if (a_attr1 != NULL) a_attr1->next = a_attr2;
   return a_attr1;
   }


/*****************************************************************************
 * Public Function _cfi_attribute_new
 *****************************************************************************/

CFI_attr_t (_cfi_attribute_new) (void* a_data, int a_type)
   {
   S_attr_t* attribute = (S_attr_t*)calloc (1, sizeof(S_attr_t));
   CFI_sym_t symbol    = sym_new();

   if ((attribute == NULL) || (symbol == NULL))
      {
      if (attribute != NULL) free (attribute);
      if (symbol    != NULL) sym_del (symbol);
      return NULL;
      }

   attribute->symbol = symbol;

   switch (a_type)
      {

      default:
         {
         free (attribute);
         sym_del (symbol);
         return NULL;
         }

      case CFI_WORD_ATTRIBUTE:
         {
         size_t l;
         char* s = cfi_string_encode (a_data, &l);
         if (s == NULL)
            {
            free (attribute);
            sym_del (symbol);
            return NULL;
            }
         sym_type_set (symbol, CFI_WORD_ATTRIBUTE);
         sym_ptr_set (symbol, s, l);
         break;
         }

      case CFI_STRING_ATTRIBUTE:
         {
         size_t l;
         char* s = cfi_string_encode (a_data, &l);
         if (s == NULL)
            {
            free (attribute);
            sym_del (symbol);
            return NULL;
            }
         sym_type_set (symbol, CFI_STRING_ATTRIBUTE);
         sym_ptr_set (symbol, s, l);
         break;
         }

      case CFI_REAL_ATTRIBUTE:
         {
         sym_type_set (symbol, CFI_REAL_ATTRIBUTE);
         sym_real_set (symbol, *(double*)a_data);
         break;
         }

      case CFI_HEX_FORMAT:
         {
         sym_type_set (symbol, CFI_HEX_FORMAT);
         sym_int_set (symbol, *(int32_t*)a_data);
         break;
         }

      case CFI_DEC_FORMAT:
         {
         sym_type_set (symbol, CFI_DEC_FORMAT);
         sym_int_set (symbol, *(int32_t*)a_data);
         break;
         }

      case CFI_OCT_FORMAT:
         {
         sym_type_set (symbol, CFI_OCT_FORMAT);
         sym_int_set (symbol, *(int32_t*)a_data);
         break;
         }

      case CFI_BIN_FORMAT:
         {
         sym_type_set (symbol, CFI_BIN_FORMAT);
         sym_int_set (symbol, *(int32_t*)a_data);
         break;
         }

      }

   return attribute;
   }


/*****************************************************************************
 * Public Function cfi_attribute_new
 *****************************************************************************/

const char* (cfi_attribute_new) (
                                CFI_attr_t* const a_attr,
                                const void*       a_data,
                                int               a_type
                                )
   {
   S_attr_t* attribute = _cfi_attribute_new ((void*)a_data, a_type);

   if (attribute == NULL) return "can't allocate memory";
   *a_attr = attribute;

   return NULL;
   }


/*****************************************************************************
 * Public Function cfi_attribute_del
 *****************************************************************************/

const char* (cfi_attribute_del) (CFI_attr_t* const a_attr)
   {
   S_attr_t* attribute = *a_attr;
   CFI_sym_t symbol    = attribute->symbol;

   if (sym_valptr(symbol) != NULL) free (sym_valptr(symbol));
   sym_del (symbol);
   free (attribute);

   return NULL;
   }


/*****************************************************************************
 * Public Function cfi_attribute_break
 *****************************************************************************/

CFI_attr_t (cfi_attribute_break) (CFI_attr_t const a_attr)
   {
   S_attr_t* next = a_attr->next;
   a_attr->next = NULL;
   return next;
   }


/*****************************************************************************
 * Public Function cfi_attribute_next
 *****************************************************************************/

CFI_attr_t (cfi_attribute_next) (CFI_attr_t const a_attr)
   {
   return a_attr->next;
   }


/*****************************************************************************
 * Public Function cfi_attribute_join
 *****************************************************************************/

CFI_attr_t (cfi_attribute_join) (
                                CFI_attr_t const a_attr1,
                                CFI_attr_t const a_attr2
                                )
   {
   if (a_attr1 != NULL) a_attr1->next = (CFI_attr_t)a_attr2;
   return a_attr1;
   }


/*****************************************************************************
 * Public Function cfi_attribute_type_get
 *****************************************************************************/

int (cfi_attribute_type_get) (CFI_attr_t const a_attr)
   {
   return sym_type (a_attr->symbol);
   }


/*****************************************************************************
 * Public Function cfi_attribute_word_get
 *****************************************************************************/

char* (cfi_attribute_word_get) (CFI_attr_t const a_attr)
   {
   if (sym_type(a_attr->symbol) != CFI_WORD_ATTRIBUTE) return NULL;
   return cfi_string_decode (
                            sym_valptr (a_attr->symbol),
                            sym_valptrlen (a_attr->symbol)
                            );
   }


/*****************************************************************************
 * Public Function cfi_attribute_string_get
 *****************************************************************************/

char* (cfi_attribute_string_get) (CFI_attr_t const a_attr)

   {
   if (a_attr->symbol->type != CFI_STRING_ATTRIBUTE) return NULL;
   return cfi_string_decode (
                            sym_valptr (a_attr->symbol),
                            sym_valptrlen (a_attr->symbol)
                            );
   }


/*****************************************************************************
 * Public Function cfi_attribute_real_get
 *****************************************************************************/

double (cfi_attribute_real_get) (CFI_attr_t const a_attr)
   {
   if (sym_type(a_attr->symbol) != CFI_REAL_ATTRIBUTE) return 0.0f;
   return sym_valreal (a_attr->symbol);
   }


/*****************************************************************************
 * Public Function CFIattributeGetIntegerNum
 *****************************************************************************/

int32_t (cfi_attribute_int_get) (CFI_attr_t const a_attr)
   {
   if ((sym_type(a_attr->symbol) & CFI_INT_ATTRIBUTE) != CFI_INT_ATTRIBUTE)
      {
      return 0;
      }
   return sym_valint (a_attr->symbol);
   }


/* end of file */
