/*
 * This file is completely free, public domain software.
 */


/* *****************************************************************************

FILE NAME

	$RCSfile:$
	$Revision:$
	$Date:$

PROGRAM INFORMATION

	Developed by:	libcfi project
	Developer:	Douglas Jerome, drj, <douglas@backstep.org>

FILE DESCRIPTION

	This is a libcfi demo/test main program that performs CFI grammer
	checking.  This main program must be linked with libcfi.

	This program reads each of the files specified on the command line and
	sucks them through the CFI grammer into an opaque libcfi structure.  It
	reports syntax errors along the way.

	Return Values

		0  Nothing to report.
		1  Bad command line option.
		2  No input file specified on the command line.
		3  Bad test result; an input file cause an error in libcfi.

CHANGE LOG

	26sep99	drj	Added the -o command line argument.
	14sep99	drj	File generation.

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
#   ifndef	_REENTRANT
#      define	_REENTRANT		/* thread-safe for glibc        */
#   endif
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
#   include	"io.h"
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
#endif
#include	<fcntl.h>

/*
 * Project Specific Header Files
 */
#include	"getopt.h"
#include	"CFI.h"


/* ************************************************************************* */
/*                                                                           */
/*      M a n i f e s t   C o n s t a n t s                                  */
/*                                                                           */
/* ************************************************************************* */

#ifdef	WIN32
#   define	STDIN_FILENO	(0)
#   define	STDOUT_FILENO	(1)
#   define	STDERR_FILENO	(2)
#endif


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

static int   g_debug;
static int   g_verbose;
static char* g_find;


/* ************************************************************************* */
/*                                                                           */
/*      E x e c u t a b l e   C o d e   (Locally Used Functions)             */
/*                                                                           */
/* ************************************************************************* */


/*****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static int find_word (CFI_node_t cfi);
static int find_attributes (CFI_node_t cfi);
static int find_section (CFI_node_t cfi);
static void help_print (void);
static int main2 (char*);


/*****************************************************************************
 * Private Function find_word
 ****************************************************************************/

static int find_word (CFI_node_t a_cfi)
   {
   int found = 0;
   CFI_node_t node = NULL;

   node = cfi_search (a_cfi, g_find, CFI_WORD);
   if (node != NULL)
      {
      found = 1;
      printf ("cfi_search: \"%s\" is a CFI_WORD.\n", g_find);
      cfi_release (node);
      }

   return found;
   }


/*****************************************************************************
 * Private Function find_attributes
 ****************************************************************************/

static int find_attributes (CFI_node_t a_cfi)
   {
   int found = 0;
   CFI_node_t node = NULL;

   node = cfi_search (a_cfi, g_find, CFI_ATTRIBUTES);
   if (node != NULL)
      {
      found = 1;
      printf ("cfi_search: \"%s\" is a CFI_ATTRIBUTES.\n", g_find);
      cfi_release (node);
      }

   return found;
   }


/*****************************************************************************
 * Private Function find_section
 ****************************************************************************/

static int find_section (CFI_node_t a_cfi)
   {
   int found = 0;
   CFI_node_t node = NULL;

   node = cfi_search (a_cfi, g_find, CFI_SECTION);
   if (node != NULL)
      {
      found = 1;
      printf ("cfi_search: \"%s\" is a CFI_SECTION.\n", g_find);
      cfi_release (node);
      }

   return found;
   }


/*****************************************************************************
 * Private Function help_print
 ****************************************************************************/

static void help_print (void)
   {
   printf ("Usage: cfichk [-options] file ...                             \n");
   printf ("Options are:                                                  \n");
   printf ("-E         Generate a command line error (funny).             \n");
   printf ("-d         Use debug mode with libcfi.                        \n");
   printf ("-f <name>  Find <name> in the cfi file.                       \n");
   printf ("-h         Display command line options, then exit.           \n");
   printf ("-v         Set verbose mode.                                  \n");
   }


/*****************************************************************************
 * Private Function main2
 ****************************************************************************/

static int main2 (char* a_fileName)
   {
   int          fd;
   CFI_node_t   cfi;
   const char*  msg;

   if (g_verbose == 1)
      {
      printf ("input file: \"%s\"\n", a_fileName);
      }

   fd = open (a_fileName, O_RDONLY);
   if (fd < 0) return -1;

   msg = cfi_init();
   if (g_verbose)
      {
      printf ("cfi_init: %s\n", msg == NULL ? "OK" : msg);
      }

   if (g_debug)
      {
      cfi_conf_debug (CFI_DEBUG_LEXICAL);
      cfi_conf_debug (CFI_DEBUG_GRAMMAR);
      }

   msg = cfi_get (fd, &cfi);
   if (g_verbose)
      {
      printf ("cfi_get: %s\n", msg == NULL ? "OK" : msg);
      }
   if (msg != NULL) return -1;

   if (g_find != NULL)
      {
      int found = 0;
      found |= find_word (cfi);
      found |= find_attributes (cfi);
      found |= find_section (cfi);
      if (!found)
         {
         printf ("cfi_search: \"%s\" is not found.\n", g_find);
         }
      }
   else
      {
      msg = cfi_put (STDOUT_FILENO, cfi);
      if (g_verbose)
         {
         printf ("cfi_put: %s\n", msg == NULL ? "OK" : msg);
         }
      }

   msg = cfi_delete_chain (cfi);
   if (g_verbose)
      {
      printf ("cfi_delete_chain: %s\n", msg == NULL ? "OK" : msg);
      }

   msg = cfi_done();
   if (g_verbose)
      {
      printf ("cfi_done: %s\n", msg == NULL ? "OK" : msg);
      }

   close (fd);

   return 0;
   }


/* ************************************************************************* */
/*                                                                           */
/*      E x e c u t a b l e   C o d e   (MAIN PROGRAM)                       */
/*                                                                           */
/* ************************************************************************* */

int   main (int argc, char** argv)
   {
   int  errNum    = 0;
   int  help      = 0;
   int  optval    = 0;
   char options[] = "Edhvf:";

   g_debug   = 0;
   g_verbose = 0;
   g_find = NULL;

   while ((optval=getopt(argc,argv,options)) != EOF)
      {
      switch (optval)
         {
         default:  fprintf (
                           stderr,
                           "cfichk: legal but unknown option `-%c'.\n",
                           optval
                           );
                   break;

         case '?':  fprintf (stderr, "cfichk: you loose 1 point.\n");
                    errNum = 1;
                    help = 1;
                    break;

         case 'd':  g_debug = g_debug == 0 ? 1 : 0;
                    break;

         case 'f':  g_find = optarg;
                    break;

         case 'h':  help = 1;
                    break;

         case 'v':  g_verbose = g_verbose == 0 ? 1 : 0;
                    break;
         }
      }

   if (g_verbose)
      {
      time_t seconds = time (NULL);
      printf ("cfichk revision: 4\n");
      printf ("//# no copyright\n");
      printf ("timestamp: %s", ctime(&seconds));
      }

   if (help || errNum)
      {
      help_print();
      exit (errNum);
      }

   if (optind >= argc)
      {
      fprintf (stderr, "cfichk: no input file.\n");
      errNum = 2;
      }
   else
      {
      int stat;
      while (optind < argc)
         {
         stat = main2 (argv[optind++]);
         if (stat) errNum = 3;
         }
      }

   if (g_verbose == 1)
      {
      printf ("error number: %d\n", errNum);
      if (errNum == 0) printf ("//# no errors.\n");
      }

   return errNum;
   }


/* end of file */
