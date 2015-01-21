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

	Name:     io.c
	Revision: 1.5
	Date:     2015-01-19

PROGRAM INFORMATION

	Developed by:	CFI project
	Developer:	Douglas Jerome, drj, <douglas@ttylinux.org>

FILE DESCRIPTION

	Configuration File Interface: I/O Implementation

	This file implements the CFI I/O functions; these functions transport
	data to/from the CFI grammer file format and the internal CFI data
	structures.

CHANGE LOG

	23jul05	drj	Gave this code another whipping.  Simplified the code
			and removed usage of libcsc.  Changed the name of the
			file from "cfi_io.c" to "io.c".

	06may02	drj	Fixup for compiling on Solaris.

	03may02	drj	Function prototype fixes. Small comment changes.

	07jul01	drj	Changed code to use new libcsc.

	24jun00	drj	Added fileName argument to CFIparseText().

	19may00	drj	File generation.

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
#   define	_XOPEN_SOURCE	500	/* for S_IFREG in <sys/stat.h> */
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
#include	<stdio.h>
#include	<stdlib.h>
#include	<time.h>

/*
 * Posix Header Files
 */
#ifndef	WIN32
#   include	<unistd.h> /* always first amongst POSIX header files */
#   include	<fcntl.h>
#   include	<sys/types.h>
#   include	<sys/stat.h>
#endif

/*
 * Project Specific Header Files
 */
#include	"CFI.h"
#include	"symbol.h"
#include	"parse.h"


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

static const char* attr_fprint (FILE* ostream, CFI_attr_t attr);
static const char* node_fprint (FILE* ostream, CFI_node_t node, int a_indent);


/*****************************************************************************
 * Private Function attr_fprint
 *****************************************************************************/

static const char* attr_fprint (FILE* a_ostream, CFI_attr_t a_attr)
   {
   char buff[(sizeof(long)*8)+4];

   if (a_attr == NULL)
      {
      fprintf (a_ostream, "/* ERROR (call 911) NULL attribute */");
      return NULL;
      }

   switch (cfi_attribute_type_get(a_attr))
      {
      default:
         {
         fprintf (a_ostream, "/* busted */");
         break;
         }
      case CFI_WORD_ATTRIBUTE:
         {
         fprintf (a_ostream, "%s", cfi_attribute_word_get(a_attr));
         break;
         }
      case CFI_STRING_ATTRIBUTE:
         {
         char* s = cfi_attribute_string_get (a_attr);
         fprintf (a_ostream, "\"%s\"", s);
         free (s);
         break;
         }
      case CFI_REAL_ATTRIBUTE:
         {
         fprintf (a_ostream, "%+12.6E", cfi_attribute_real_get(a_attr));
         break;
         }
      case CFI_HEX_FORMAT:
         {
         fprintf (
                 a_ostream,
                 "0x%08lX",
                 (unsigned long)cfi_attribute_int_get(a_attr)
                 );
         break;
         }
      case CFI_DEC_FORMAT:
         {
         fprintf (a_ostream, "%ld", (long)cfi_attribute_int_get(a_attr));
         break;
         }
      case CFI_OCT_FORMAT:
         {
         fprintf (
                 a_ostream,
                 "%s",
                 cfi_string_octal (buff, (long)cfi_attribute_int_get(a_attr))
                 );
         }
         break;
      case CFI_BIN_FORMAT:
         {
         fprintf (
                 a_ostream,
                 "%s",
                 cfi_string_binary (buff, (long)cfi_attribute_int_get(a_attr))
                 );
         break;
         }
      }

   return NULL;
   }


/*****************************************************************************
 * Private Function node_fprint
 *****************************************************************************/

static const char* node_fprint (
                               FILE*      a_ostream,
                               CFI_node_t a_node,
                               int        a_indent
                               )
   {
   int       i;

   while (a_node != NULL)
      {

      /*
       * Print the word.
       */
      if (cfi_node_is_deleted(a_node))
         fprintf (a_ostream, "--DELETED ");
      else
         for (i = 0 ; i < a_indent ; i++) fputc (' ', a_ostream);
      fprintf (a_ostream, "%s", cfi_node_word(a_node));

      if (cfi_node_type_get(a_node) == CFI_WORD)
         {
         /*
          * The form is just the word; end the statement.
          */
         fprintf (a_ostream, ";\n");
         }

      if (cfi_node_type_get(a_node) == CFI_ATTRIBUTES)
         {
         /*
          * The form is a word-attribute; print the attribute(s).
          */
         CFI_attr_t attribute = cfi_node_attribute (a_node);
         fprintf (a_ostream, " = ");
         do
            {
            attr_fprint (a_ostream, attribute);
            if (attribute != NULL) attribute = cfi_attribute_next (attribute);
            if (attribute != NULL) fprintf (a_ostream, ", ");
            }
         while (attribute != NULL);
         fprintf (a_ostream, ";\n");
         }

      if (cfi_node_type_get(a_node) == CFI_SECTION)
         {
         /*
          * The form is a CFI "section" ; print the parameter if it is present,
          * and then recursively print the contents.
          */
         /* Print the parameter if it is present. */
         if (cfi_node_attribute(a_node) != NULL)
            {
            fprintf (a_ostream, " (");
            attr_fprint (a_ostream, cfi_node_attribute(a_node));
            fprintf (a_ostream, ")");
            }
         fprintf (a_ostream, "\n");
         a_indent += 3;
         /* Print opening '{' for the contents. */
         if (cfi_node_is_deleted(a_node))
            fprintf (a_ostream, "--DELETED ");
         else
            for (i = 0 ; i < a_indent ; i++) fputc (' ', a_ostream);
         fprintf (a_ostream, "{\n");
         /* Recursively print the contents. */
         if (cfi_node_section(a_node) != NULL)
            node_fprint (a_ostream, cfi_node_section(a_node), a_indent);
         else
            {
            if (cfi_node_is_deleted(a_node))
               fprintf (a_ostream, "--DELETED (empty)\n");
            else
               {
               for (i = 0 ; i < a_indent ; i++) fputc (' ', a_ostream);
               fprintf (a_ostream, "-- empty\n");
               }
            }
         /* Print closing '}' for the contents. */
         if (cfi_node_is_deleted(a_node))
            fprintf (a_ostream, "--DELETED ");
         else
            for (i = 0 ; i < a_indent ; i++) fputc (' ', a_ostream);
         fprintf (a_ostream, "}\n");
         a_indent -= 3;
         }

      a_node = cfi_node_next (a_node);
      }

   return NULL;
   }


/* ************************************************************************* */
/*                                                                           */
/*      E x e c u t a b l e   C o d e   (External Interface Functions)       */
/*                                                                           */
/* ************************************************************************* */


/*****************************************************************************
 * Public Function cfi_get
 *****************************************************************************/

const char* (cfi_get) (int a_fd, CFI_node_t* const a_node)
   {

#ifdef	_unix
   {
   struct stat fstatBuff;

   if (fstat(a_fd,&fstatBuff) == -1)
      {
      return "can't get status on input";
      }

   if (fstatBuff.st_mode & S_IFREG)
      {
      /*
       * The input is a file; try reading the entire file into a dynamically
       * allocated "char" buffer.
       */
      char* buff = (char*)calloc (1, fstatBuff.st_size+32);
      int   size;
      if (buff == NULL)
         {
         return "memory allocation error";
         }
      size = read (a_fd, buff, fstatBuff.st_size);
      if (size == fstatBuff.st_size)
         {
         buff[fstatBuff.st_size] = '\0';
         *a_node = cfi_parse_text (buff); /* Load the data from the file. */
         free (buff);
         return NULL; /* Assumes success. */
         }
      free (buff);
      /*
       * At this point, read() didn't successfully read the entire file, so
       * try to lseek() back to the begining of the file.
       */
      {
      off_t seekStat = lseek (a_fd, (off_t)0, SEEK_SET);
      if (seekStat != 0)
         {
         return "lost input reference";
         }
      }
      }
   }
#endif

   /* Try reading with stream I/O. */
   {
   FILE* istream = fdopen (a_fd, "r");
   if (istream == NULL)
      {
      return "can't create input stream";
      }
   *a_node = cfi_parse_file (istream);
   }

   return NULL; /* Assumes success. */
   }


/*****************************************************************************
 * Public Function cfi_put
 *****************************************************************************/

const char* (cfi_put) (int a_fd, CFI_node_t  const a_node)
   {
   FILE*  ostream = fdopen (a_fd, "w");
   time_t seconds   = time (NULL);

   if (ostream == NULL) return "can't create output stream";

   (void)fprintf (ostream, "//# file updated:  %s", ctime(&seconds));
   (void)fprintf (ostream, "//# libcfi version %s\n\n", cfi_conf_version());

   return node_fprint (ostream, a_node, 0);
   }


/* end of file */
