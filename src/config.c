/*
 * This file is part of the CFI software.
 * The license which this software falls under is as follows:
 *
 * Copyright (C) 2001-2005 Douglas Jerome <douglas@ttylinux.org>
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


/* ****************************************************************************

FILE NAME

	Name:     config.c
	Revision: 1.8
	Date:     2015-01-19

PROGRAM INFORMATION

	Developed by:	CFI project
	Developer:	Douglas Jerome, drj, <douglas@ttylinux.org>

FILE DESCRIPTION

	Configuration File Interface: CFI Configuration

	libcfi configuration functions.

CHANGE LOG

	23jul05	drj	Gave this code a whipping.  Simplified the code and
			removed usage of libcsc.  Changed the name of the
			file from "cfi_config.c" to "config.c".

	03may02	drj	Small comment changes.

	25jun01	drj	File generation.

**************************************************************************** */


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
/* (none) */

/*
 * Posix Header Files
 */
#ifndef	WIN32
#   include	<unistd.h> /* always first amongst POSIX header files */
#endif

/*
 * Project Specific Header Files
 */
#if	defined(AUTOCONF)
#   include	"../_AUTOCONF/config.h"
#elif	defined(WIN32)
#   include	"../_MSVC6/win32_config.h"
#elif	defined(_unix)
#   include	"make_config.h"
#else
#   error  ***** Unknown build configuration.
#endif
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

int CFI_debugLexical;
int CFI_debugGrammar;


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
 * Public Config Functions
 *****************************************************************************/

const char* (cfi_conf_credits) (void)
   {
   return LIBCFI_COPYRIGHT;
   }

const char* (cfi_conf_version) (void)     { return LIBCFI_REVISION;  }
const char* (cfi_conf_date) (void)        { return LIBCFI_TIMESTAMP; }
const char* (cfi_conf_cflags) (void)      { return CLIENT_CFLAGS;    }
const char* (cfi_conf_shared_libs) (void) { return CLIENT_SHARLIBS;  }
const char* (cfi_conf_static_libs) (void) { return CLIENT_STATLIBS;  }

unsigned (cfi_conf_debug) (unsigned a_flags)
   {
   unsigned flags = 0;

   if (CFI_debugLexical) flags |= CFI_DEBUG_LEXICAL;
   if (CFI_debugGrammar) flags |= CFI_DEBUG_GRAMMAR;

   CFI_debugLexical = (a_flags & CFI_DEBUG_LEXICAL) != 0;
   CFI_debugGrammar = (a_flags & CFI_DEBUG_GRAMMAR) != 0;

   return flags;
   }


/*****************************************************************************
 * Public Function cfi_init
 *****************************************************************************/

const char* (cfi_init) (void)
   {
   CFI_debugLexical = 0;
   CFI_debugGrammar = 0;
   return NULL;
   }


/*****************************************************************************
 * Public Function cfi_done
 *****************************************************************************/

const char* (cfi_done) (void)
   {
   return NULL;
   }


#ifdef  WIN32

/*****************************************************************************
 * Public Function DllMain
 *****************************************************************************/

BOOL APIENTRY DllMain (HANDLE hModule, DWORD ulReasonForCall, LPVOID lpReserved)
   {
   switch (ulReasonForCall)
      {
      default: break;
      case DLL_PROCESS_ATTACH: break;
      case DLL_PROCESS_DETACH: break;
      case DLL_THREAD_ATTACH:  break;
      case DLL_THREAD_DETACH:  break;
      }
   return TRUE;
   }

#endif


/* end of file */
