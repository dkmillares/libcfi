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

	Name:     data_node.c
	Revision: 1.5
	Date:     2015-01-19

PROGRAM INFORMATION

	Developed by:	CFI project
	Developer:	Douglas Jerome, drj, <douglas@ttylinux.org>

FILE DESCRIPTION

	Configuration File Interface: Node Implementation

	This file contains the CFI functions that create, destroy, query, and
	otherwise directly manipulate CFI nodes; these are the nodes that are
	the word, word-attribute and section of CFI data.

CHANGE LOG

	24jul05	drj	Culled these CFI node functions from a larger file
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

typedef struct S_node_t
   {
   struct S_node_t*  pred;
   struct S_node_t*  next;
   int               discriminator;
   char*             word;
   size_t            attributeCount;
   CFI_attr_t        attributeList;
   CFI_attr_t*       attributeLink;
   struct S_node_t*  contents;
   int               changed;
   int               deleted;
   size_t            retainCount;
   }
   S_node_t;

typedef int (*CFI_callback_t) (S_node_t* const);


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

static int node_delete (S_node_t* const node);
static int node_release (S_node_t* const node);
static int node_retain (S_node_t* const node);
static __inline__ int node_whack (S_node_t* const node);

static int cfi_traverse (S_node_t* const node, CFI_callback_t cbfn);

static __inline__ void cfi_whack (S_node_t* const node);


/*****************************************************************************
 * Private Function node_delete
 *****************************************************************************
 *
 * This function should not be inlined because is executed as a function
 * argument to cfi_traverse().
 *
 * This function sets the delete flag of a node.
 *
 * Return Value
 *
 *     0 - Indicates that the node cannot be deleted, because its retain count
 *         is not zero.
 *
 *     1 - Indicates that the node can be deleted, because its retain count is
 *         zero.
 *
 *****************************************************************************/

static int node_delete (S_node_t* const a_node)
   {
   a_node->deleted = 1;
   if (a_node->retainCount == 0) return 1;
   return 0;
   }


/*****************************************************************************
 * Private Function node_release
 *****************************************************************************
 *
 * This function should not be inlined because is executed as a function
 * argument to cfi_traverse().
 *
 * This function decrements the retain count of a node, if needed.
 *
 * Return Value
 *
 *     0 - Indicates that the node is not completed released i.e., the node's
 *         retain count is not zero.
 *
 *     1 - Indicates that the node is completed released i.e., the node's
 *         retain count iS zero.
 *
 *****************************************************************************/

static int node_release (S_node_t* const a_node)
   {
   if (a_node->retainCount > 0) a_node->retainCount -= 1;
   if (a_node->retainCount == 0) return 1;
   return 0;
   }


/*****************************************************************************
 * Private Function node_retain
 *****************************************************************************
 *
 * This function should not be inlined because is executed as a function
 * argument to cfi_traverse().
 *
 * This function increments the retain count of a node.
 *
 * Return Value
 *
 *     0 - Indicates that the node is not retained by this function.
 *
 *     1 - Indicates that the node is retained by this function.
 *
 *****************************************************************************/

static int node_retain (S_node_t* const a_node)
   {
   if (a_node->deleted) return 0;
   a_node->retainCount += 1;
   return 1;
   }


/*****************************************************************************
 * Private Function node_whack
 *****************************************************************************
 *
 * This function removes a node from the tree and deallocates all allocated
 * memory associated with the node.
 *
 *****************************************************************************/

static __inline__ int node_whack (S_node_t* const a_node)
   {
   S_node_t* node = (S_node_t*)a_node;

   /*
    * 1.  unlink node
    *
    * [a] The predesessor of this node becomes the predesessor of the next node,
    *     if there is a next node.
    *
    * [b] If the predessor's contents are this node, then this node is the
    *     first (perhaps only) node of the the contents of the presessor node.
    *     Make the contents of the predesessor become the next node.
    *
    * [c] If the predessor's next node is this node, then make the predessor's
    *     next node be this node's next node.
    */
   if (node->next != NULL) node->next->pred = node->pred; /* [a] */
   if (node->pred != NULL)
      {
      if (node->pred->contents == node) /* [b] */
         {
         node->pred->contents = node->next;
         }
      else if (node->pred->next == node) /* [c] */
         {
         node->pred->next = node->next;
         }
      }

   /*
    * 2.  deallocate node
    */
   (void)cfi_node_attribute_del (node); /* Deallocate any attributes. */
   (void)cfi_node_word_del(node); /* Deallocate the word. */
   (void)cfi_node_del (&node); /* Deallocate the node. */

   return 0;
   }


/*****************************************************************************
 * Private Function cfi_traverse
 *****************************************************************************/

static int cfi_traverse (S_node_t* const a_node, CFI_callback_t a_cbfn)
   {
   int       stat = 0;
   S_node_t* node;

   node = a_node;
   while (node != NULL)
      {
      if (node->contents != NULL) stat &= cfi_traverse (node->contents, a_cbfn);
      stat &= (*a_cbfn)(node);
      node = node->next;
      }

   return stat;
   }


/*****************************************************************************
 * Private Function cfi_whack
 *****************************************************************************
 *
 * This function removes a node from the tree and deallocates all allocated
 * memory associated with the node.  If the node is a "section", then ALL of
 * the nodes that are the contents are also whacked.
 *
 *****************************************************************************/

static __inline__ void cfi_whack (S_node_t* const a_node)
   {
   S_node_t* node = a_node->contents;
   S_node_t* p;

   (void)node_whack (a_node);

   while (node != NULL)
      {
      p = node->next;
      (void)node_whack (node);
      node = p;
      }

   return;
   }


/* ************************************************************************* */
/*                                                                           */
/*      E x e c u t a b l e   C o d e   (External Interface Functions)       */
/*                                                                           */
/* ************************************************************************* */


/*****************************************************************************
 * Public Function _cfi_node_join
 *****************************************************************************/

CFI_node_t
__attribute__ ((visibility("hidden")))
(_cfi_node_join) (CFI_node_t a_node1, CFI_node_t a_node2)
   {
   if (a_node2 != NULL) a_node2->pred = a_node1;
   if (a_node1 != NULL) a_node1->next = a_node2;
   return a_node1;
   }


/*****************************************************************************
 * Public Function _cfi_node_word_new
 *****************************************************************************/

CFI_node_t (_cfi_node_word_new) (char* a_word)
   {
   S_node_t*   node;
   const char* stat;

   stat = cfi_node_new (&node);
   if (stat != NULL) return NULL; /* Dynamic memory allocation failure. */

   (void)cfi_node_word_set (node, a_word); /* Since the node was just created */
                                           /* this should not fail.  See the  */
                                           /* function below.                 */

   return node;
   }


/*****************************************************************************
 * Public Function _cfi_node_attribute_new
 *****************************************************************************/

CFI_node_t (_cfi_node_attribute_new) (char* a_word, CFI_attr_t a_attr)
   {
   S_node_t*   node;
   const char* stat;

   stat = cfi_node_new (&node);
   if (stat != NULL) return NULL; /* Dynamic memory allocation failure. */

   (void)cfi_node_type_set (node, CFI_ATTRIBUTES); /* Since the node was just */
   (void)cfi_node_word_set (node, a_word);         /* created these functions */
   (void)cfi_node_attribute_set (node, a_attr);    /* should not fail.  See   */
                                                   /* the functions below.    */

   return node;
   }


/*****************************************************************************
 * Public Function _cfi_node_section_new
 *****************************************************************************/

CFI_node_t (_cfi_node_section_new) (
                                   char*      a_word,
                                   CFI_attr_t a_attr,
                                   CFI_node_t a_contents
                                   )
   {
   S_node_t*   node;
   const char* stat;

   stat = cfi_node_new (&node);
   if (stat != NULL) return NULL; /* Dynamic memory allocation failure. */

   (void)cfi_node_type_set (node, CFI_SECTION);   /* Since the node was just */
   (void)cfi_node_word_set (node, a_word);        /* created these functions */
   (void)cfi_node_attribute_set (node, a_attr);   /* should not fail.  See   */
   (void)cfi_node_section_set (node, a_contents); /* the functions below.    */

   return node;
   }


/*****************************************************************************
 * Public Function cfi_node_new
 *****************************************************************************/

const char* (cfi_node_new) (CFI_node_t* const a_node)
   {
   S_node_t* node = (S_node_t*)calloc (1, sizeof(S_node_t));

   if (node == NULL) return "can't allocate memory";

   node->pred           = NULL;
   node->next           = NULL;
   node->discriminator  = CFI_WORD;
   node->word           = NULL;
   node->attributeCount = 0;
   node->attributeList  = NULL;
   node->attributeLink  = NULL;
   node->contents       = NULL;
   node->changed        = 0;
   node->deleted        = 0;
   node->retainCount    = 0;

   *a_node = node;

   return NULL;
   }


/*****************************************************************************
 * Public Function cfi_node_del
 *****************************************************************************/

const char* (cfi_node_del) (CFI_node_t* const a_node)
   {
   free (*a_node);
   return NULL;
   }


/*****************************************************************************
 * Public Function cfi_node_type_get
 *****************************************************************************/

int (cfi_node_type_get) (CFI_node_t const a_node)
   {
   return a_node->discriminator;
   }


/*****************************************************************************
 * Public Function cfi_node_type_set
 *****************************************************************************/

const char* (cfi_node_type_set) (CFI_node_t const a_node, int a_type)
   {
   if ((a_type != CFI_WORD) &&
       (a_type != CFI_ATTRIBUTES) &&
       (a_type != CFI_SECTION))
      {
      return "invalid type";
      }
   a_node->discriminator = a_type;
   return NULL;
   }


/*****************************************************************************
 * Public Function cfi_node_break
 *****************************************************************************/

CFI_node_t (cfi_node_break) (CFI_node_t const a_node)
   {
   S_node_t* next = a_node->next;
   a_node->next = NULL;
   return next;
   }


/*****************************************************************************
 * Public Function cfi_node_next
 *****************************************************************************/

CFI_node_t (cfi_node_next) (CFI_node_t const a_node)
   {
   return a_node->next;
   }


/*****************************************************************************
 * Public Function cfi_node_join
 *****************************************************************************/

CFI_node_t (cfi_node_join) (CFI_node_t const a_node1, CFI_node_t const a_node2)
   {
   if (a_node2 != NULL) a_node2->pred = (CFI_node_t)a_node1;
   if (a_node1 != NULL) a_node1->next = (CFI_node_t)a_node2;
   return a_node1;
   }


/*****************************************************************************
 * Public Function cfi_node_word
 *****************************************************************************/

char* (cfi_node_word) (CFI_node_t const a_node)
   {
   return a_node->word;
   }


/*****************************************************************************
 * Public Function cfi_node_word_get
 *****************************************************************************/

const char* (cfi_node_word_get) (CFI_node_t const a_node, char** const a_word)
   {
   *a_word = a_node->word;
   return NULL;
   }


/*****************************************************************************
 * Public Function cfi_node_word_set
 *****************************************************************************/

const char* (cfi_node_word_set) (CFI_node_t const a_node, char* const a_word)
   {
   if (a_node->deleted) return "node is already deleted";
   if (a_node->word != NULL) return "word already set";
   a_node->word = a_word;
   return NULL;
   }


/*****************************************************************************
 * Public Function cfi_node_word_del
 *****************************************************************************/

const char* (cfi_node_word_del) (CFI_node_t const a_node)
   {
   if (a_node->word == NULL) return "there is no word";
   free (a_node->word);
   a_node->word = NULL;
   return NULL;
   }


/*****************************************************************************
 * Public Function cfi_node_attribute_count
 *****************************************************************************/

size_t (cfi_node_attribute_count) (CFI_node_t const a_node)
   {
   return a_node->attributeCount;
   }


/*****************************************************************************
 * Public Function cfi_node_attribute
 *****************************************************************************/

CFI_attr_t (cfi_node_attribute) (CFI_node_t const a_node)
   {
   return a_node->attributeList;
   }


/*****************************************************************************
 * Public Function cfi_node_attribute_get
 *****************************************************************************/

const char* (cfi_node_attribute_get) (
                                     CFI_node_t  const a_node,
                                     CFI_attr_t* const a_attr
                                     )
   {
   *a_attr = a_node->attributeList;
   return NULL;
   }


/*****************************************************************************
 * Public Function cfi_node_attribute_set
 *****************************************************************************/

const char* (cfi_node_attribute_set) (
                                     CFI_node_t const a_node,
                                     CFI_attr_t const a_attr
                                     )
   {
   CFI_attr_t* attrArray;
   CFI_attr_t  attr;
   int        i;

   if (a_node->deleted) return "node is already deleted";
   if (a_node->discriminator == CFI_WORD) return "wrong node type";
   if (a_node->attributeList != NULL) return "attribute already set";
   if (a_attr == NULL) return NULL;

   attr = a_attr;
   i    = 0;
   while (attr != NULL)
      {
      i += 1;
      attr = cfi_attribute_next (attr);
      }

   attrArray = (CFI_attr_t*)calloc (i, sizeof(CFI_attr_t));
   if (attrArray == NULL) return "can't allocate memory";

   attr = a_attr;
   i    = 0;
   while (attr != NULL)
      {
      attrArray[i] = attr;
      attr = cfi_attribute_next (attr);
      }

   a_node->attributeCount = i;
   a_node->attributeList  = a_attr;
   a_node->attributeLink  = attrArray;

   return NULL;
   }


/*****************************************************************************
 * Public Function cfi_node_attribute_del
 *****************************************************************************/

const char* (cfi_node_attribute_del) (CFI_node_t const a_node)
   {
   CFI_attr_t attr;

   if (a_node->attributeList == NULL) return "there is no attribute";

   attr = a_node->attributeList;
   while (attr != NULL)
      {
      attr = cfi_attribute_next (attr);
      (void)cfi_attribute_del (&attr);
      }

   free (a_node->attributeLink);

   a_node->attributeCount = 0;
   a_node->attributeList  = NULL;
   a_node->attributeLink  = NULL;

   return NULL;
   }


/*****************************************************************************
 * Public Function cfi_node_attribute_insert
 *****************************************************************************/

const char* (cfi_node_attribute_insert) (
                                        CFI_node_t const a_node,
                                        size_t           a_offset,
                                        CFI_attr_t const a_attr
                                        )
   {
   CFI_attr_t* attrArray;
   CFI_attr_t  p;
   int         i;

   if (a_node->deleted) return "node is already deleted";
   if (a_node->discriminator == CFI_WORD) return "wrong node type";
   if (a_offset > a_node->attributeCount) return "offset too big";

   attrArray = (CFI_attr_t*)calloc(a_node->attributeCount+1,sizeof(CFI_attr_t));
   if (attrArray == NULL) return "can't allocate memory";

   if (a_offset == 0)
      {
      (void)cfi_attribute_join (a_attr, a_node->attributeList);
      a_node->attributeList = a_attr;
      }
   else
      {
      p = a_node->attributeLink[a_offset-1];
      (void)cfi_attribute_join (a_attr, cfi_attribute_next(p));
      (void)cfi_attribute_join (p, a_attr);
      }

   for (p=a_node->attributeList, i=0 ; p!=NULL ; p=cfi_attribute_next(p))
      {
      attrArray[i++] = p;
      }

   free (a_node->attributeLink);
   a_node->attributeLink = attrArray;

   a_node->attributeCount += 1;

   return NULL;
   }



/*****************************************************************************
 * Public Function cfi_node_attribute_remove
 *****************************************************************************/

const char* (cfi_node_attribute_remove) (
                                        CFI_node_t  const a_node,
                                        size_t            a_offset,
                                        CFI_attr_t* const a_attr
                                        )
   {
   CFI_attr_t* attrArray;
   CFI_attr_t  p;
   size_t      i;

   if (a_node->deleted) return "node is already deleted";
   if (a_node->discriminator == CFI_WORD) return "wrong node type";
   if (a_offset >= a_node->attributeCount) return "offset too big";

   attrArray = (CFI_attr_t*)calloc(a_node->attributeCount-1,sizeof(CFI_attr_t));
   if (attrArray == NULL) return "can't allocate memory";

   if (a_offset == 0)
      {
      *a_attr = a_node->attributeList;
      a_node->attributeList = cfi_attribute_next (a_node->attributeList);
      }
   else
      {
      p = a_node->attributeLink[a_offset-1];
      *a_attr = cfi_attribute_next (p);
      (void)cfi_attribute_join (p, cfi_attribute_next(cfi_attribute_next(p)));
      }

   for (p=a_node->attributeList, i=0 ; p!=NULL ; p=cfi_attribute_next(p))
      {
      attrArray[i++] = p;
      }

   free (a_node->attributeLink);
   a_node->attributeLink = attrArray;

   a_node->attributeCount -= 1;

   return NULL;
   }


/*****************************************************************************
 * Public Function cfi_node_section
 *****************************************************************************/

CFI_node_t (cfi_node_section) (CFI_node_t const a_node)
   {
   return a_node->contents;
   }


/*****************************************************************************
 * Public Function cfi_node_section_get
 *****************************************************************************/

const char* (cfi_node_section_get) (
                                   CFI_node_t  const a_node,
                                   CFI_node_t* const a_contents
                                   )
   {
   *a_contents = a_node->contents;
   return NULL;
   }


/*****************************************************************************
 * Public Function cfi_node_section_set
 *****************************************************************************/

const char* (cfi_node_section_set) (
                                   CFI_node_t const a_node,
                                   CFI_node_t const a_contents
                                   )
   {
   if (a_node->deleted) return "node is already deleted";
   if (a_node->contents != NULL) return "section already set";
   if (a_contents == NULL) return NULL;

   a_node->contents = a_contents;
   a_contents->pred = a_node;

   return NULL;
   }


/*****************************************************************************
 * Public Function cfi_search
 *****************************************************************************/

CFI_node_t (cfi_search) (
                        CFI_node_t const a_node,
                        const char*      a_word,
                        int              a_type
                        )
   {
   S_node_t* node = a_node;
   S_node_t* item = NULL;

   while (node != NULL)
      {
      if (CFI_STREQ(node->word,a_word) && (node->discriminator == a_type))
         item = cfi_retain (node) == node ? node : NULL;
      else
         {
         if (node->discriminator == CFI_SECTION)
            item = cfi_search (node->contents, a_word, a_type);
         }
      if (item != NULL)
         node = NULL;
      else
         node = node->next;
      }

   return item;
   }


/*****************************************************************************
 * Public Function cfi_search_flat
 *****************************************************************************/

CFI_node_t (cfi_search_flat) (
                             CFI_node_t const a_node,
                             const char*      a_word,
                             int              a_type
                             )
   {
   S_node_t* node = a_node;
   S_node_t* item = NULL;

   while (node != NULL)
      {
      if (CFI_STREQ(node->word,a_word) && (node->discriminator == a_type))
         {
         item = cfi_retain (node) == node ? node : NULL;
         }
      if (item != NULL)
         node = NULL;
      else
         node = node->next;
      }

   return item;
   }


/*****************************************************************************
 * Public Function cfi_retain
 *****************************************************************************
 *
 * This function increments the retain count for a node and, if the node is a
 * "section", ALL of the nodes that are the contents.
 *
 *****************************************************************************/

CFI_node_t (cfi_retain) (CFI_node_t a_node)
   {
   if (a_node->deleted) return NULL;
   if (a_node->discriminator == CFI_SECTION)
      (void)cfi_traverse (a_node->contents, node_retain);
   a_node->retainCount += 1;
   return a_node;
   }


/*****************************************************************************
 * Public Function cfi_release
 *****************************************************************************/

const char* (cfi_release) (CFI_node_t a_node)
   {
   int allNodesReleased = 0; /* This indicates whether or not all nodes   */
                             /* involved in the release become completely */
                             /* released.  This happens when their retain */
                             /* count becomes zero upon being released.   */

   if (a_node->retainCount == 0) return "not retained";

   a_node->retainCount -= 1;
   if (a_node->retainCount == 0) allNodesReleased = 1;

   if (a_node->discriminator == CFI_SECTION)
      {
      allNodesReleased &= cfi_traverse (a_node->contents, node_release);
      }

   if (allNodesReleased && (a_node->deleted)) cfi_whack (a_node);

   return NULL;
   }


/*****************************************************************************
 * Public Function cfi_delete
 *****************************************************************************/

const char* (cfi_delete) (CFI_node_t a_node)
   {
   int allNodesDeletable = 0; /* This indicates whether or not all nodes     */
                              /* involved in the delete become completely    */
                              /* deletable.  This happens when their retain  */
                              /* count is zero upon having their delete flag */
                              /* being set.                                  */
   a_node->deleted = 1;
   if (a_node->retainCount == 0) allNodesDeletable = 1;

   if (a_node->discriminator == CFI_SECTION)
      {
      allNodesDeletable &= cfi_traverse (a_node->contents, node_delete);
      }

   if (allNodesDeletable) cfi_whack (a_node);

   return NULL;
   }


/*****************************************************************************
 * Public Function cfi_delete_chain
 *****************************************************************************/

const char* (cfi_delete_chain) (CFI_node_t a_node)
   {
   int allNodesDeletable = 1; /* This indicates whether or not all nodes     */
                              /* involved in the delete become completely    */
                              /* deletable.  This happens when their retain  */
                              /* count is zero upon having their delete flag */
                              /* being set.                                  */

   S_node_t* node;
   S_node_t* p;

   node = a_node;
   while (node != NULL)
      {
      a_node->deleted = 1;
      if (a_node->retainCount > 0) allNodesDeletable = 0;

      if (node->discriminator == CFI_SECTION)
         {
         allNodesDeletable &= cfi_traverse (node->contents, node_delete);
         }
      node = node->next;
      }

   if (allNodesDeletable)
      {
      node = a_node;
      while (node != NULL)
         {
         p = node->next;
         cfi_whack (node);
         node = p;
         }
      }

   return NULL;
   }


/*****************************************************************************
 * Public Function cfi_node_is_deleted
 *****************************************************************************/

int (cfi_node_is_deleted) (CFI_node_t a_node)
   {
   return a_node->deleted;
   }


/* end of file */
