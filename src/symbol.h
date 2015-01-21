/*
 * This file is part of the CFI software.
 * The license which this software falls under is as follows:
 *
 * Copyright (C) 2005 Douglas Jerome <douglas@ttylinux.org>
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

	Name:     symbol.h
	Revision: 1.6
	Date:     2015-01-19

PROJECT INFORMATION

	Developed by:	CFI project
	Maintainer:	Douglas Jerome, drj, <douglas@ttylinux.org>

FILE DESCRIPTION

	Configuration File Interface:

	libcfi Private symbol data type and operations.

CHANGE LOG

	04jun06	drj	Fixed "SYMBOL_H" to be "CFI_SYMBOL_H".

	03aug05	drj	File generation.

***************************************************************************** */


#ifndef CFI_SYMBOL_H
#define CFI_SYMBOL_H 1


#ifdef	__cplusplus
extern	"C"	{
#endif


/* ************************************************************************* */
/*                                                                           */
/*      I n c l u d e d   F i l e s                                          */
/*                                                                           */
/* ************************************************************************* */

#include	<stdlib.h>
#ifdef	_unix
#   define	_POSIX_C_SOURCE	200112L /* posix.1 and posix.4 and MORE */
#   include	<unistd.h> /* always first amongst POSIX header files */
#endif
#include	"CFI.h"


/* ************************************************************************* */
/*                                                                           */
/*      M a n i f e s t   C o n s t a n t s                                  */
/*                                                                           */
/* ************************************************************************* */

/* (none) */


/* ************************************************************************* */
/*                                                                           */
/*      D a t a   T y p e s   a n d   S t r u c t u r e s                    */
/*                                                                           */
/* ************************************************************************* */

typedef union U_sym_t
   {
   long   integer;
   double real;
   void*  pointer;
   }
   U_sym_t;

typedef struct S_sym_t
   {
   int     type;
   size_t  dvsize;
   U_sym_t value;
   }
   S_sym_t;


/* ************************************************************************* */
/*                                                                           */
/*      P u b l i c   G l o b a l   V a r i a b l e s                        */
/*                                                                           */
/* ************************************************************************* */

/* (none) */


/* ************************************************************************* */
/*                                                                           */
/*      I n l i n e   F u n c t i o n s                                      */
/*                                                                           */
/* ************************************************************************* */


/*****************************************************************************
 * Inline Function Prototypes
 *****************************************************************************/

static __inline__ S_sym_t* sym_new (void);
static __inline__ void sym_del (S_sym_t* a_sym);
static __inline__ size_t sym_valptrlen (S_sym_t* a_sym);
static __inline__ void* sym_valptr (S_sym_t* a_sym);
static __inline__ double sym_valreal (S_sym_t* a_sym);
static __inline__ int32_t sym_valint (S_sym_t* a_sym);
static __inline__ int sym_type (S_sym_t* a_sym);
static __inline__ void sym_type_set (S_sym_t* a_sym, int a_val);
static __inline__ void sym_int_set (S_sym_t* a_sym, int32_t a_val);
static __inline__ void sym_real_set (S_sym_t* a_sym, double a_val);
static __inline__ void sym_ptr_set (S_sym_t* a_sym, void* a_val, size_t a_len);


/*****************************************************************************
 * Inline new, del Functions
 *****************************************************************************/

static __inline__ S_sym_t* sym_new (void)
   {
   return (S_sym_t*)calloc (1, sizeof(S_sym_t));
   }

static __inline__ void sym_del (S_sym_t* a_sym)
   {
   free (a_sym);
   }


/*****************************************************************************
 * Inline get Functions
 *****************************************************************************/

static __inline__ int sym_type (S_sym_t* a_sym)
   {
   return a_sym->type;
   }

static __inline__ int32_t sym_valint (S_sym_t* a_sym)
   {
   return a_sym->value.integer;
   }

static __inline__ double sym_valreal (S_sym_t* a_sym)
   {
   return a_sym->value.real;
   }

static __inline__ void* sym_valptr (S_sym_t* a_sym)
   {
   return a_sym->value.pointer;
   }

static __inline__ size_t sym_valptrlen (S_sym_t* a_sym)
   {
   return a_sym->dvsize;
   }


/*****************************************************************************
 * Inline set Functions
 *****************************************************************************/

static __inline__ void sym_type_set (S_sym_t* a_sym, int a_val)
   {
   a_sym->type = a_val;
   }

static __inline__ void sym_int_set (S_sym_t* a_sym, int32_t a_val)
   {
   a_sym->value.integer = a_val;
   }

static __inline__ void sym_real_set (S_sym_t* a_sym, double a_val)
   {
   a_sym->value.real = a_val;
   }

static __inline__ void sym_ptr_set (S_sym_t* a_sym, void* a_val, size_t a_len)
   {
   a_sym->value.pointer = a_val;
   a_sym->dvsize = a_len;
   }


#ifdef	__cplusplus
}
#endif


#endif
