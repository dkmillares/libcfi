/*
 * This file is part of the CFI software.
 * The license which this software falls under is as follows:
 *
 * Copyright (C) 2000-2015 Douglas Jerome <douglas@ttylinux.org>
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

	Name:     CFI.h
	Revision: 1.11
	Date:     2015-01-19

PROGRAM INFORMATION

	Developed by:	CFI project
	Developer:	Douglas Jerome, drj, <douglas@ttylinux.org>

FILE DESCRIPTION

	Configuration File Interface: Common Constants, Data Types, etc.

	This is the main libcfi header file.  This file contains libcfi external
	data and functions that are exported for libcfi client code.

CHANGE LOG

	18jan15	drj	Removed the buggy compile-time check for NULL's value.

	11jun06	drj	Fixed the definition of NULL.

	07jun06	drj	Fixed "ASSERT" to be "CFI_ASSERT".

	04jun06	drj	Mindless finessing.

	07oct05	drj	Mindless text format fix.

	03aug05	drj	Added int32_t.

	21jul05	drj	Simplified the code, added win32 capability and changed
			the name of this file from "libcfi.h" to "CFI.h".

	03may02	drj	Fixed up function prototypes. Small comment changes.

	07jul01	drj	Changed code to use new libcsc.

	12mar00	drj	Added functions for getting to the attributes and their
			values.

	20jun00	drj	File generation.

***************************************************************************** */


#ifndef CFI_H
#define CFI_H 1


#ifdef	__cplusplus
extern	"C"	{
#endif


/* ************************************************************************* */
/*                                                                           */
/*      I n c l u d e d   F i l e s                                          */
/*                                                                           */
/* ************************************************************************* */

#ifdef	C99
#   include	<stdint.h>
#endif
#include	<string.h>
#ifdef	_unix
#   undef	_POSIX_C_SOURCE
#   define	_POSIX_C_SOURCE	200112L /* posix.1 and posix.4 and MORE */
#   include	<unistd.h> /* always first amongst POSIX header files */
#endif


/* ************************************************************************* */
/*                                                                           */
/*      M a n i f e s t   C o n s t a n t s                                  */
/*                                                                           */
/* ************************************************************************* */

#ifdef	_WIN32
#   undef	WIN32
#   define	WIN32
#endif

/*
 * Setup proper export, import keyword(s) for some compiler environments.
 * This fun stuff is nicked from SDL (www.libsdl.org).
 */
#undef	DECLS
#ifdef	WIN32
#   ifdef	CFI_EXPORTS
#      define	DECLS	_declspec(dllexport)
#   else
#      define	DECLS	__declspec(dllimport)
#   endif
#else
#   define	DECLS
#endif

/*
 * Setup proper C calling convention keyword(s) for some compiler environments.
 * This fun stuff is nicked from SDL (www.libsdl.org).
 */
#undef	DECLC
#ifdef	WIN32
#   define	DECLC	_cdecl
#else
#   define	DECLC
#endif

/*
 * Set up a proper compiler-specific inline directive.  This fun stuff is
 * nicked from SDL (www.libsdl.org).
 */
#undef CFI_INLINE_OKAY
#ifdef __GNUC__
#   define CFI_INLINE_OKAY
#else
#   if defined(_MSC_VER)    || defined(__BORLANDC__) || \
       defined(__DMC__)     || defined(__SC__)       || \
       defined(__WATCOMC__) || defined(__LCC__)
#      define CFI_INLINE_OKAY
#      define __inline__ __inline
#   else
#      if !defined(__MRC__) && !defined(_SGI_SOURCE)
#         define CFI_INLINE_OKAY
#         define __inline__ inline
#      endif
#   endif
#endif
#ifndef CFI_INLINE_OKAY
#   define __inline__
#endif

/*
 * Once, I didn't have a NULL.  It is a sad thing to not have a NULL.
 */
#ifndef	NULL
#   ifndef	__cplusplus
#      define	NULL	((void*)0)
#   else
#      define	NULL	(0)
#   endif
#endif

/*
 * CFI_node_t discriminators:
 */
#define	CFI_WORD		(0x574F5244)
#define	CFI_ATTRIBUTES		(0x41545452)
#define	CFI_SECTION		(0x53454354)

/*
 * Some symbolic names for CFI attributes:
 */
#define	CFI_WORD_ATTRIBUTE	(0x10)
#define	CFI_STRING_ATTRIBUTE	(0x20)
#define	CFI_REAL_ATTRIBUTE	(0x30)
#define	CFI_INT_ATTRIBUTE	(0x40)

/*
 * Some symbolic names for CFI integer attribute formats (these all imply and
 * are variants of the CFI attribute "CFI_INT_ATTRIBUTE"):
 */
#define	CFI_HEX_FORMAT		(0x41)
#define	CFI_DEC_FORMAT		(0x42)
#define	CFI_OCT_FORMAT		(0x43)
#define	CFI_BIN_FORMAT		(0x44)

/*
 * Debug Flags
 */
#define	CFI_DEBUG_LEXICAL	((unsigned)(1<<0))
#define	CFI_DEBUG_GRAMMAR	((unsigned)(1<<1))

/*
 * Everyone's got these.
 */
#define	CFI_OK			(0)
#define	CFI_ERR			(-1)
#define	CFI_STREQ(s1,s2)	(strcmp(s1,s2)==0)
#define	CFI_STRNEQ(s1,s2)	(strcmp(s1,s2)!=0)


/* ************************************************************************* */
/*                                                                           */
/*      D a t a   T y p e s   a n d   S t r u c t u r e s                    */
/*                                                                           */
/* ************************************************************************* */

/*
 *
 */
#ifndef	SOLARIS
#ifndef	__BIT_TYPES_DEFINED__
typedef   int   int32_t;
#endif
#endif
/*
 * This is a compile-time check for int32_t being four bytes in size.  If
 * int32_t is not four bytes in size, then these next lines should create a
 * compile error.
 */
#ifdef	_MSC_VER
typedef int CFI_CHECK_UINT32_1[4-sizeof(int32_t)+1];
typedef int CFI_CHECK_UINT32_2[sizeof(int32_t)-4+1];
#else
typedef int CFI_CHECK_UINT32_1[4-sizeof(int32_t)];
typedef int CFI_CHECK_UINT32_2[sizeof(int32_t)-4];
#endif

/*
 * Usefull commentary goes here.
 */
typedef  struct S_sym_t*   CFI_sym_t;
typedef  struct S_attr_t*  CFI_attr_t;
typedef  struct S_node_t*  CFI_node_t;


/* ************************************************************************* */
/*                                                                           */
/*      P u b l i c   G l o b a l   V a r i a b l e s                        */
/*                                                                           */
/* ************************************************************************* */

extern int CFI_debugLexical;
extern int CFI_debugGrammar;


/* ************************************************************************* */
/*                                                                           */
/*      F u n c t i o n   P r o t o t y p e s                                */
/*                                                                           */
/* ************************************************************************* */

/*
 * This is how many of the CFI functions are declared.  Probably all of them
 * should be declared this way.
 */
#define   CFI_FUNC   extern DECLS const char* DECLC

/* -- CFI Configuration Function Prototypes */

extern DECLS const char* DECLC cfi_conf_credits (void);
extern DECLS const char* DECLC cfi_conf_version (void);
extern DECLS const char* DECLC cfi_conf_date (void);
extern DECLS const char* DECLC cfi_conf_cflags (void);
extern DECLS const char* DECLC cfi_conf_shared_libs (void);
extern DECLS const char* DECLC cfi_conf_static_libs (void);
extern DECLS unsigned DECLC cfi_conf_debug (unsigned flags);

/* -- CFI Initialization Function Prototypes */

extern DECLS const char* DECLC cfi_init (void);
extern DECLS const char* DECLC cfi_done (void);

/* -- CFI String Function Prototypes */

extern DECLS char* DECLC cfi_string_decode (const char* text, size_t leng);
extern DECLS char* DECLC cfi_string_encode (const char* text, size_t* leng);
extern DECLS char* DECLC cfi_string_octal  (char* const a_buff, long a_item);
extern DECLS char* DECLC cfi_string_binary (char* const a_buff, long a_item);

/* -- CFI I/O Function Prototypes */

extern DECLS const char* DECLC cfi_get (int fd, CFI_node_t* const node);
extern DECLS const char* DECLC cfi_put (int fd, CFI_node_t  const node);

/* -- CFI Allocation, Deallocation Function Prototypes */

CFI_FUNC cfi_node_new (CFI_node_t* const node);
CFI_FUNC cfi_node_del (CFI_node_t* const node);
CFI_FUNC cfi_attribute_new (CFI_attr_t* const attr, const void* data, int type);
CFI_FUNC cfi_attribute_del (CFI_attr_t* const attr);

/* -- CFI Node Manipulation Function Prototypes */

extern DECLS int DECLC cfi_node_type_get (CFI_node_t const node);
CFI_FUNC cfi_node_type_set (CFI_node_t const node, int  type);
extern DECLS CFI_node_t DECLC cfi_node_break (CFI_node_t const node);
extern DECLS CFI_node_t DECLC cfi_node_next (CFI_node_t const node);
extern DECLS CFI_node_t DECLC cfi_node_join (
                                            CFI_node_t const node1,
                                            CFI_node_t const node2
                                            );

/* -- CFI Node Word Manipulation Function Prototypes */

extern DECLS char* DECLC cfi_node_word (CFI_node_t const node);
CFI_FUNC cfi_node_word_get (CFI_node_t const node, char** const word);
CFI_FUNC cfi_node_word_set (CFI_node_t const node, char*  const word);
CFI_FUNC cfi_node_word_del (CFI_node_t const node);

/* -- CFI Node Attribute Manipulation Function Prototypes */

extern DECLS size_t DECLC cfi_node_attribute_count (CFI_node_t const node);
extern DECLS CFI_attr_t DECLC cfi_node_attribute (CFI_node_t const node);
CFI_FUNC cfi_node_attribute_get (CFI_node_t const node, CFI_attr_t* const attr);
CFI_FUNC cfi_node_attribute_set (CFI_node_t const node, CFI_attr_t  const attr);
CFI_FUNC cfi_node_attribute_del (CFI_node_t const node);
CFI_FUNC cfi_node_attribute_insert (
                                   CFI_node_t const node,
                                   size_t           offset,
                                   CFI_attr_t const attr
                                   );
CFI_FUNC cfi_node_attribute_remove (
                                   CFI_node_t  const node,
                                   size_t            offset,
                                   CFI_attr_t* const attr
                                   );

/* -- CFI Node Section Manipulation Function Prototypes */

extern DECLS CFI_node_t DECLC cfi_node_section (CFI_node_t const node);
CFI_FUNC cfi_node_section_get (
                              CFI_node_t  const node,
                              CFI_node_t* const contents
                              );
CFI_FUNC cfi_node_section_set (
                              CFI_node_t const node,
                              CFI_node_t const contents
                              );

/* -- CFI Attribute Manipulation Function Prototypes */

extern DECLS CFI_attr_t DECLC cfi_attribute_break(CFI_attr_t const attr);
extern DECLS CFI_attr_t DECLC cfi_attribute_next (CFI_attr_t const attr);
extern DECLS CFI_attr_t DECLC cfi_attribute_join (
                                                 CFI_attr_t const attr1,
                                                 CFI_attr_t const attr2
                                                 );

/* -- CFI Query Node Function Prototypes */

extern DECLS CFI_node_t DECLC cfi_search (
                                         CFI_node_t const node,
                                         const char*      word,
                                         int              type
                                         );
extern DECLS CFI_node_t DECLC cfi_search_flat (
                                              CFI_node_t const node,
                                              const char*      word,
                                              int              type
                                              );
extern DECLS CFI_node_t  DECLC cfi_retain (CFI_node_t node);
extern DECLS const char* DECLC cfi_release (CFI_node_t node);
extern DECLS const char* DECLC cfi_delete (CFI_node_t node);
extern DECLS const char* DECLC cfi_delete_chain (CFI_node_t node);
extern DECLS int DECLC cfi_node_is_deleted (CFI_node_t node);

/* -- CFI Query Attribute Function Prototypes */

extern DECLS int     DECLC cfi_attribute_type_get (CFI_attr_t const attr);
extern DECLS char*   DECLC cfi_attribute_word_get (CFI_attr_t const attr);
extern DECLS char*   DECLC cfi_attribute_string_get (CFI_attr_t const attr);
extern DECLS double  DECLC cfi_attribute_real_get (CFI_attr_t const attr);
extern DECLS int32_t DECLC cfi_attribute_int_get (CFI_attr_t const attr);

/* -- CFI Parse's Function Prototypes (DON'T USE THESE) */

extern CFI_node_t _cfi_node_join (CFI_node_t, CFI_node_t);
extern CFI_node_t _cfi_node_word_new (char*);
extern CFI_node_t _cfi_node_attribute_new (char*, CFI_attr_t);
extern CFI_node_t _cfi_node_section_new (char*, CFI_attr_t, CFI_node_t);
extern CFI_attr_t _cfi_attribute_join (CFI_attr_t, CFI_attr_t);
extern CFI_attr_t _cfi_attribute_new (void*, int);

#undef	CFI_FUNC


/* ************************************************************************* */
/*                                                                           */
/*      D e b u g   S u p p o r t                                            */
/*                                                                           */
/* ************************************************************************* */

#ifdef	_DEBUG
#   undef	DEBUG
#   define	DEBUG
#endif

#undef	CFI_ASSERT
#define	CFI_ASSERT(exp)	((void)0)
#ifdef	DEBUG
#   include	<stdio.h>
#   undef	CFI_ASSERT
#   define	CFI_ASSERT(exp)						\
        {if (!(exp)) {							\
        (void)fflush (stdout);						\
        (void)fflush (stderr);						\
        (void)fprintf(stderr,						\
        "Assertion failed: FILE %s, LINE %u, EXPRESSION \"%s\".\n",	\
        __FILE__,__LINE__,#exp);					\
        (void)fflush (stderr); }}
#endif


#ifdef	__cplusplus
}
#endif


#endif
