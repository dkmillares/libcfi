/*
 * This file is part of the CFI software.
 * The license which this software falls under is as follows:
 *
 * Copyright (C) 2000-2005 Douglas Jerome <douglas@ttylinux.org>
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

	Name:     string.c
	Revision: 1.5
	Date:     2015-01-19

PROGRAM INFORMATION

	Developed by:	CFI project
	Developer:	Douglas Jerome, drj, <douglas@ttylinux.org>

FILE DESCRIPTION

	Configuration File Interface: String Handling

	String handling functions taken from libPropList.  Someone else
	(Bjoern Giesler?) deserves credit for this code.

CHANGE LOG

	21jul05	drj	Removed use of libcsc.  Changed the name of this file
			from "cfi_string.c" to "string.c".

	03may02	drj	Small comment changes.

	07jul01	drj	Changed code to use new libcsc.

	27may00	drj	File generation.

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

/* (none) */


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
 * Public Function cfi_string_decode
 *****************************************************************************/

char* (cfi_string_decode) (const char* a_text, size_t a_leng)
   {
#define	INRANGE(ch,min,max)	((ch)>=(min) && (ch)<=(max))
#define	ESC_CHAR(ch)	(INRANGE(ch,07,015) || ((ch)=='"') || ((ch)=='\\'))
#define	ESC_NUM(ch)	(((ch)<=06) || INRANGE(ch,016,037) || ((ch)>0176))

         char*  newtext = NULL;
         char*  dst;
   const char*  src;
         size_t size;

   if (a_text == NULL) return NULL;

   /*
    * This code seems to account for when a_text is only '\0' (the string ""),
    * and a_leng is (correctly) 1.
    */

   {
   size_t i; /* This is just a counter; count for a_leng-1 because a_leng is */
             /* the size of a_text including its terminating '\0'.           */
   for (i=1, src=a_text, size=1 ; i < a_leng ; i++, src++, size++)
      if (ESC_CHAR(*src))
         size += 1;
      else
         if (ESC_NUM(*src))
            size += 3;
   }

   newtext = (char*)calloc (1, size+1);
   if (newtext == NULL) return NULL;

   dst = newtext;
   src = a_text;

   while (--a_leng > 0)
      {
      if (ESC_CHAR(*src) || (*src == '\0'))
         {
         *dst++ = '\\';
         switch (*src)
            {
            default:    *dst++ =  *src;  break;
            case '\0':  *dst++ =  '0';   break;
            case '\a':  *dst++ =  'a';   break;
            case '\b':  *dst++ =  'b';   break;
            case '\f':  *dst++ =  'f';   break;
            case '\n':  *dst++ =  'n';   break;
            case '\r':  *dst++ =  'r';   break;
            case '\t':  *dst++ =  't';   break;
            case '\v':  *dst++ =  'v';   break;
            }
         src++;
         }
      else
         {
         if (ESC_NUM(*src))
            {
            *dst++ = '\\';
            *dst++ = '0'+((*src>>6)&07);
            *dst++ = '0'+((*src>>3)&07);
            *dst++ = '0'+( *src    &07);
            src++;
            }
         else
            *dst++ = *src++;
         }
      }

   *dst = '\0';

   return newtext;

#undef	INRANGE
#undef	ESC_CHAR
#undef	ESC_NUM
   }


/*****************************************************************************
 * Public Function cfi_string_encode
 *****************************************************************************/

char* (cfi_string_encode) (const char* a_text, size_t* a_leng)
   {
         char* newtext = NULL;
         char* dst;
   const char* src;

   if (a_text == NULL) return NULL;

   newtext = (char*)calloc (1, strlen(a_text)+1);
   if (newtext == NULL) return NULL;

   dst = newtext;
   src = a_text;

   while (*src != '\0')
      {
      if (*src != '\\') *dst++ = *src++;
      else
         {
         src++;
         if ((*src >= '0') && (*src <= '2'))
            {
            unsigned char byte = 0;
            byte += (*src++ & 07);
            if ((*src >= '0') && (*src <= '7'))
               {
               byte = (byte << 3) + (*src++ & 07);
               if ((*src >= '0') && (*src <= '7'))
                  byte = (byte << 3) + (*src++ & 07);
               }
            *dst++ = byte;
            }
         else
            {
            switch (*src)
               {
               default:    *dst++ = *src;  break;
               case  '0':  *dst++ = '\0';  break;
               case  'a':  *dst++ = '\a';  break;
               case  'b':  *dst++ = '\b';  break;
               case  'f':  *dst++ = '\f';  break;
               case  'n':  *dst++ = '\n';  break;
               case  'r':  *dst++ = '\r';  break;
               case  't':  *dst++ = '\t';  break;
               case  'v':  *dst++ = '\v';  break;
               }
            src++;
            }
         }
      }

   *dst++ = '\0';
   if (a_leng != NULL) *a_leng = dst - newtext;

   return newtext;
   }


/*****************************************************************************
 * Public Function cfi_string_octal
 *****************************************************************************/

char* (cfi_string_octal) (char* const a_buff, long a_item)
   {
   char* octalString;
   long  mask;
   int   shift;

   if (a_buff != NULL)
      {
      octalString = &a_buff[((sizeof(long)*8)/3)+3];
      *octalString-- = '\0';
      for (mask = 0x07, shift = 0 ; mask != 0 ; mask = mask << 3, shift += 3)
         {
         *octalString-- = '0'+ (char)((a_item & mask) >> shift);
         }
      *octalString-- = 'o';
      *octalString-- = '0';
      }

   return a_buff;
   }


/*****************************************************************************
 * Public Function cfi_string_binary
 *****************************************************************************/

char* (cfi_string_binary) (char* const a_buff, long a_item)
   {
   char* binaryString;
   long  mask;

   binaryString = &a_buff[(sizeof(long)*8)+2];
   *binaryString-- = '\0';
   for (mask=0x01 ; mask!=0 ; mask=mask<<1)
      {
      if ((a_item & mask) == mask)
         *binaryString-- = '1';
      else
         *binaryString-- = '0';
      }
   *binaryString-- = 'b';
   *binaryString-- = '0';

   return a_buff;
   }


/* end of file */
