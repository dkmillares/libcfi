# This file is part of the CFI software.
# The license which this software falls under is as follows:
#
# Copyright (C) 2005-2015 Douglas Jerome <douglas@ttylinux.org>
#
# This library is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 2.1 of the License, or (at
# your option) any later version.
#
# This library is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
# License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation,
# Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

# *****************************************************************************
#
# FILE NAME
#
#	Name:     Makefile
#	Revision: 1.15
#	Date:     2010-01-19
#
# PROJECT INFORMATION
#
#	Developed by:	CFI project
#	Maintainer:	Douglas Jerome, drj, <douglas@ttylinux.org>
#
# FILE DESCRIPTION
#
#	Configuration File Interface: makefile for the source code directory.
#
# CHANGE LOG
#
#       18jan15 drj     Removed solaris support.
#			Added ld script to control symbols.
#			Added V=1 command line option.
#                       Changed INSTALL_INCLUDE and INSTALL_SHARE directory
#                       names from ${NAMELC} to ${NAMELC}-${MAJOR}.
#                       Added $(CROSS_COMPILE).
#                       Removed the RCS usage.
#                       Added the 'help' make target, removed the 'source' and
#                       'objects' help targets.
#                       Changed the handling of optimization and debug flags.
#
#	02feb07	drj	Fixed INSTALLPREFIX to be INSTALL_PREFIX.
#			More mindless finessing.
#
#	08jun06	drj	CFI version 1.0.1
#			Adjusted flex and ld flags for Solaris.
#
#	04jun06	drj	Mindless finessing.  Changed "PROJ" to "CFI".  Added
#			.l.c: and .y.c: rules.
#
#	04jun06	drj	Mindless finessing.
#
#	27sep05	drj	Fixed OSTYPE.  Fixed compiler command line.
#
#	26sep05	drj	Fixed header and source file names.
#
#	21jul05	drj	File generation.
#
# *****************************************************************************

# Command Arguments used by this Makefile
#
#	V=1			Use 'make V=1' to see the command lines.
#
#	CROSS_COMPILE=<name>	Use CROSS_COMPILE to specify the target build
#				tool prefix; gcc and bin-utils are prefixed
#				with $(CROSS_COMPILE).
#				CROSS_COMPILE can be set on the command line
#				eg,
#				'make CROSS_COMPILE=powerpc-e500v2-linux-gnu-'
#				or CROSS_COMPILE can be set in the environment.
#
#	CC_PARAMS=<options>	This can be used to specify gcc debug or
#				optimization flags.  If more than one flag, use
#				like these:
#				'CC_PARAMS=-O3 -mfloat-gprs=double'
#				'CC_PARAMS=-DDEBUG -g'
#
#	INSTALL_PREFIX=<path>	Use to specify the sys-root of where to put the
#				SCL libraries; the default is /usr/local, but
#				this should be used to point to the target
#				install directory.

# *************************************************************************** #
# Macro Definitions                                                           #
# *************************************************************************** #

# -- Product Configuration
#
MAJOR		= 1
MINOR		= 1
SUBLEVEL	= 0
VERSION		= ${MAJOR}.${MINOR}
FULL_VERSION	= ${MAJOR}.${MINOR}.${SUBLEVEL}

# -- Make Configuration
#
ifeq ($(filter --no-print-directory,$(MAKEFLAGS)),)
MAKEFLAGS	+= --no-print-directory
endif
ifeq ($(V),1)
Q		=
else
Q		= @
endif
export Q

# -- Host Build Environment
#
DATE	= $(shell date)

# -- Product Name
#
NUMEUC	= CFI
NAMELC	= $(shell echo ${NUMEUC} | tr '[:upper:]' '[:lower:]')

# -- Input Files
#
HEADERS	=		\
	CFI.h		\
	parse.h		\
	lex.h		\
	symbol.h
OBJECTS	=		\
	config.o	\
	string.o	\
	parse.o		\
	lex.o		\
	data_attr.o	\
	data_node.o	\
	io.o
SOURCES	=		\
	config.c	\
	string.c	\
	parse.y		\
	lex.l		\
	data_attr.c	\
	data_node.c	\
	io.c

# -- Generated Files
#
CONFIG	= make_config.h
CONFIGN	= $(shell echo ${CONFIG} | tr '.' '_' | tr '[:lower:]' '[:upper:]')
CFICFG	= ${NAMELC}-config
ARCHIVE	= lib${NAMELC}.a
LIBRARY	= lib${NAMELC}.so
SONAME	= lib${NAMELC}.so.${MAJOR}
SONAMEV	= lib${NAMELC}.so.${MAJOR}.${MINOR}.${SUBLEVEL}

# -- Install Locations
#
ifeq ("${INSTALL_PREFIX}","")
INSTALL_PREFIX	= /usr/local
endif
INSTALL_BIN	= ${INSTALL_PREFIX}/bin
INSTALL_INCLUDE	= ${INSTALL_PREFIX}/include/${NAMELC}-${MAJOR}
INSTALL_LIB	= ${INSTALL_PREFIX}/lib
INSTALL_LIBEXEC	= ${INSTALL_PREFIX}/libexec
INSTALL_SHARE	= ${INSTALL_PREFIX}/share/${NAMELC}-${MAJOR}

# *************************************************************************** #
# More Macro Definitions                                                      #
# *************************************************************************** #

# -- General Host Tools
#
CAT	= cat
CHMOD	= chmod
CP	= cp
ECHO	= echo
FILE	= file
INSTALL	= install -c
LINK	= ln -fns
LS	= ls --color -l
MV	= mv
RM	= rm -f
SED	= sed
LEX	= lex
YACC	= yacc

# -- Target Development Tools
#
AR	= $(CROSS_COMPILE)ar
CC	= $(CROSS_COMPILE)gcc
LD	= $(CROSS_COMPILE)ld
RANLIB	= $(CROSS_COMPILE)ranlib
STRIP	= $(CROSS_COMPILE)strip
SZ	= $(CROSS_COMPILE)size

# -- cpp Flags
#
CPP_DEFINES	= -D_unix -DLINUX -D${NUMEUC}_EXPORTS
CPP_INCLUDES	= -iquote.
CPP_FLAGS	= -MMD ${CPP_DEFINES} ${CPP_INCLUDES}

# -- cc Flags
#
CC_COMPILER	= -ansi -c -fPIC -pipe ${CC_PARAMS} ${OPTIMIZ} -fno-common
CC_WARNING	= -Wall -W -Wcast-align -Wshadow
CC_WARNING2	= -Wmissing-prototypes -Wmissing-declarations
CC_FLAGS	= ${CC_COMPILER} ${CC_WARNING} ${CC_WARNING2}

# -- ld Flags
#
LIBS		= -lc
LD_SONAME_FLAGS	= -shared -Wl,-soname=${SONAME},--version-script=ld-export.map

# -- flex (lex) Flags
#
LFLAGS		= -t --nounistd

# -- bison (yacc) Flags
#
YFLAGS		= -d -y

# -- Client Flags
#
CLIENT_CFLAGS	= -I${INSTALL_INCLUDE}
CLIENT_SHARLIBS	= -L${INSTALL_LIB} -Wl,-rpath,${INSTALL_LIB} -l${NAMELC} ${LIBS}
CLIENT_STATLIBS	= -L${INSTALL_LIB} -l${NAMELC} ${LIBS}

# *************************************************************************** #
# Make Rules                                                                  #
# *************************************************************************** #

.c.o:
	@${ECHO} "CC	$<"
	$(Q)${CC} ${CPP_FLAGS} ${CC_FLAGS} $<

.l.c:
	@${ECHO} "LEX	$<"
	$(Q)${LEX} ${LFLAGS} $< > ${<:.l=.c}
	$(Q)${SED} -i ${<:.l=.c} -e 's/^\(#include *<stdio.h>\)/#define _POSIX_SOURCE\n\1/'

.l.o:
	@${ECHO} "LEX	$<"
	$(Q)${LEX} ${LFLAGS} $< > ${<:.l=.c}
	$(Q)${SED} -i ${<:.l=.c} -e 's/^\(#include *<stdio.h>\)/#define _POSIX_SOURCE\n\1/'
	@${ECHO} "CC	${<:.l=.c}"
	$(Q)${CC} ${CPP_FLAGS} ${CC_FLAGS} ${<:.l=.c}
	@${RM} ${<:.l=.c}

.y.c:
	@${ECHO} "YACC	$<"
	$(Q)${YACC} ${YFLAGS} $<
	@${MV} y.tab.c ${<:.y=.c}

.y.o:
	@${ECHO} "YACC	$<"
	$(Q)${YACC} ${YFLAGS} $<
	@${MV} y.tab.c ${<:.y=.c}
	@${ECHO} "CC	${<:.y=.c}"
	$(Q)${CC} ${CPP_FLAGS} ${CC_FLAGS} ${<:.y=.c}
	@${RM} ${<:.y=.c}


# *************************************************************************** #
# Make Targets                                                                #
# *************************************************************************** #

.PHONY:	default
.PHONY:	testh
.PHONY:	clean config
.PHONY:	install uninstall
.PHONY:	debug optimize libs

# -----------------------------------------------------------------------------

# -- Default Target
# -----------------------------------------------------------------------------

default:	libs

# -----------------------------------------------------------------------------
# -- Debug Targets
# -----------------------------------------------------------------------------

testh:	${HEADERS}
	@for file in ${HEADERS}; do					\
		cfile=_`expr $${file} : "\(.*\)\.h"`.c;			\
		dfile=_`expr $${file} : "\(.*\)\.h"`.d;			\
		ofile=_`expr $${file} : "\(.*\)\.h"`.o;			\
		${RM} $${cfile} $${dfile};				\
		${ECHO} "#include \"$${file}\""    >> $${cfile};	\
		${ECHO} "#include \"$${file}\""    >> $${cfile};	\
		${ECHO} "#include \"$${file}\""    >> $${cfile};	\
		${ECHO} "int main() { return 0; }" >> $${cfile};	\
		make $${ofile};						\
		${RM} $${cfile} $${dfile} $${ofile};			\
	done

# -----------------------------------------------------------------------------
# -- Housekeeping Targets
# -----------------------------------------------------------------------------

clean:
	@${ECHO} "RM	*.d .depend"
	@${ECHO} "RM	y.tab.* lex.yy.* lex.c parse.c"
	@${ECHO} "RM	${CONFIG} ${CFICFG}"
	@${ECHO} "RM	OBJECTS ${ARCHIVE} ${LIBRARY} ${LIBRARY}.*"
	@${RM} ${OBJECTS:.o=.d} .depend
	@${RM} y.tab.* lex.yy.* lex.c parse.c
	@${RM} ${CONFIG} ${CFICFG} ${OBJECTS} ${ARCHIVE} ${LIBRARY} ${LIBRARY}.*

config ${CONFIG}:	Makefile ${CFICFG}
	@${ECHO} "RM	${CONFIG}"
	@${RM} ${CONFIG}
	@${ECHO} "CAT	${CONFIG}"
	@${ECHO} "/*"                                              >> ${CONFIG}
	@${ECHO} " * This file is generated by Makefile.src"       >> ${CONFIG}
	@${ECHO} " * Edits to this file will be lost."             >> ${CONFIG}
	@${ECHO} " */"                                             >> ${CONFIG}
	@${ECHO} ""                                                >> ${CONFIG}
	@${ECHO} "#ifndef ${NAMEUC}_${CONFIGN}"                    >> ${CONFIG}
	@${ECHO} "#define ${NAMEUC}_${CONFIGN} 1"                  >> ${CONFIG}
	@${ECHO} ""                                                >> ${CONFIG}
	@${ECHO} "static const char* const LIBCFI_COPYRIGHT = \"Copyright (C) 2005-2015 Douglas Jerome <douglas@ttylinux.org>\";" >> ${CONFIG}
	@${ECHO} "static const char* const LIBCFI_TIMESTAMP = __TIME__ \", \" __DATE__;"                                          >> ${CONFIG}
	@${ECHO} "static const char* const LIBCFI_REVISION  = \"vers. ${VERSION}\";"                                              >> ${CONFIG}
	@${ECHO} ""                                                >> ${CONFIG}
	@${ECHO} "#define CLIENT_CFLAGS   \"${CLIENT_CFLAGS}\""    >> ${CONFIG}
	@${ECHO} "#define CLIENT_SHARLIBS \"${CLIENT_SHARLIBS}\""  >> ${CONFIG}
	@${ECHO} "#define CLIENT_STATLIBS \"${CLIENT_STATLIBS}\""  >> ${CONFIG}
	@${ECHO} ""                                                >> ${CONFIG}
	@${ECHO} "#define MAKE_INSTALLDIR \"${INSTALL_PREFIX}\""   >> ${CONFIG}
	@${ECHO} "#define MAKE_BINDIR     \"${INSTALL_BIN}\""      >> ${CONFIG}
	@${ECHO} "#define MAKE_INCLUDEDIR \"${INSTALL_INCLUDE}\""  >> ${CONFIG}
	@${ECHO} "#define MAKE_LIBDIR     \"${INSTALL_LIB}\""      >> ${CONFIG}
	@${ECHO} "#define MAKE_LIBEXECDIR \"${INSTALL_LIBEXEC}\""  >> ${CONFIG}
	@${ECHO} "#define MAKE_SHAREDIR   \"${INSTALL_SHARE}\""    >> ${CONFIG}
	@${ECHO} ""                                                >> ${CONFIG}
	@${ECHO} "#endif"                                          >> ${CONFIG}

${CFICFG}:	Makefile
	@${ECHO} "RM	${CFICFG}"
	@${RM} ${CFICFG}
	@${ECHO} "CAT	${CFICFG}"
	@${ECHO} '#!/bin/sh'                                       >> ${CFICFG}
	@${ECHO} ''                                                >> ${CFICFG}
	@${ECHO} "prefix=\"${INSTALL_PREFIX}\""                    >> ${CFICFG}
	@${ECHO} "credits=\"Copyright (C) 2005-2015 Douglas Jerome <douglas@ttylinux.org>\"" >> ${CFICFG}
	@${ECHO} "version=\"${FULL_VERSION}\""                     >> ${CFICFG}
	@${ECHO} "cflags=\"${CLIENT_CFLAGS}\""                     >> ${CFICFG}
	@${ECHO} "sharedlibs=\"${CLIENT_SHARLIBS}\""               >> ${CFICFG}
	@${ECHO} "staticlibs=\"${CLIENT_STATLIBS}\""               >> ${CFICFG}
	@${ECHO} ''                                                >> ${CFICFG}
	@${ECHO} 'usage="Usage: cfi-config [--prefix] [--credits] [--version] [--cflags] [--shared-libs] [--static-libs]"' >> ${CFICFG}
	@${ECHO} ''                                                >> ${CFICFG}
	@${ECHO} 'if test $$# -eq 0; then'                         >> ${CFICFG}
	@${ECHO} '	echo "$${usage}" 1>&2'                     >> ${CFICFG}
	@${ECHO} '	exit 1'                                    >> ${CFICFG}
	@${ECHO} 'fi'                                              >> ${CFICFG}
	@${ECHO} ''                                                >> ${CFICFG}
	@${ECHO} 'while test $$# -gt 0; do'                        >> ${CFICFG}
	@${ECHO} '	case $$1 in'                               >> ${CFICFG}
	@${ECHO} '		--prefix)'                         >> ${CFICFG}
	@${ECHO} '			echo $${prefix}'           >> ${CFICFG}
	@${ECHO} '			;;'                        >> ${CFICFG}
	@${ECHO} '		--credits)'                        >> ${CFICFG}
	@${ECHO} '			echo $${credits}'          >> ${CFICFG}
	@${ECHO} '			;;'                        >> ${CFICFG}
	@${ECHO} '		--version)'                        >> ${CFICFG}
	@${ECHO} '			echo $${version}'          >> ${CFICFG}
	@${ECHO} '			;;'                        >> ${CFICFG}
	@${ECHO} '		--cflags)'                         >> ${CFICFG}
	@${ECHO} '			echo $${cflags}'           >> ${CFICFG}
	@${ECHO} '			;;'                        >> ${CFICFG}
	@${ECHO} '		--shared-libs)'                    >> ${CFICFG}
	@${ECHO} '			echo $${sharedlibs}'       >> ${CFICFG}
	@${ECHO} '			;;'                        >> ${CFICFG}
	@${ECHO} '		--static-libs)'                    >> ${CFICFG}
	@${ECHO} '			echo $${staticlibs}'       >> ${CFICFG}
	@${ECHO} '			;;'                        >> ${CFICFG}
	@${ECHO} '		*)'                                >> ${CFICFG}
	@${ECHO} '			echo "$$1: huh?" 1>&2'     >> ${CFICFG}
	@${ECHO} '			echo "$${usage}" 1>&2'     >> ${CFICFG}
	@${ECHO} '			exit 1'                    >> ${CFICFG}
	@${ECHO} '			;;'                        >> ${CFICFG}
	@${ECHO} '	esac'                                      >> ${CFICFG}
	@${ECHO} '	shift'                                     >> ${CFICFG}
	@${ECHO} 'done'                                            >> ${CFICFG}
	@${ECHO} ''                                                >> ${CFICFG}
	@${ECHO} 'exit 0'                                          >> ${CFICFG}
	@${CHMOD} 755 ${CFICFG}

# -----------------------------------------------------------------------------
# -- Installation Targets
# -----------------------------------------------------------------------------

install:
	${INSTALL} -D -m 644 CFI.h ${INSTALL_INCLUDE}/CFI.h
	${INSTALL} -m 644 ${ARCHIVE} ${INSTALL_LIB}
	${INSTALL} -m 755 ${SONAMEV} ${INSTALL_LIB}
	${LINK} ${SONAMEV} ${INSTALL_LIB}/${LIBRARY}
	${LINK} ${SONAMEV} ${INSTALL_LIB}/${SONAME}
	${INSTALL} -m 755 ${CFICFG} ${INSTALL_BIN}

uninstall:
	${RM} -r ${INSTALL_INCLUDE}
	${RM} ${INSTALL_LIB}/${ARCHIVE}
	${RM} ${INSTALL_LIB}/${LIBRARY}
	${RM} ${INSTALL_LIB}/${SONAME}
	${RM} ${INSTALL_LIB}/${SONAMEV}
	${RM} ${INSTALL_BIN}/${CFICFG}

# -----------------------------------------------------------------------------
# -- Build Targets
# -----------------------------------------------------------------------------

debug:
	${MAKE} ${MAKEFLAGS} clean
	${MAKE} ${MAKEFLAGS} 'CC_PARAMS=-DDEBUG -g'

optimize:
	${MAKE} ${MAKEFLAGS} clean
	${MAKE} ${MAKEFLAGS} 'CC_PARAMS=-O3'

libs:	${CFICFG} ${CONFIG} ${HEADERS} ${SOURCES} ${OBJECTS}
	@${ECHO} "CAT	.depend"
	@${RM} .depend
	@${CAT} ${OBJECTS:.o=.d} > .depend
	@${ECHO} "AR	${ARCHIVE}"
	@${RM} ${ARCHIVE}
	$(Q)${AR} r ${ARCHIVE} ${OBJECTS} 2> /dev/null
	$(Q)${RANLIB} ${ARCHIVE}
	@${CHMOD} 644 ${ARCHIVE}
	@${ECHO} "LD	 ${SONAMEV}"
	@${RM} ${LIBRARY} ${SONAME} ${SONAMEV}
	$(Q)${CC} ${LD_SONAME_FLAGS} -o ${SONAMEV} ${OBJECTS} ${LIBS}
	@${LINK} ${SONAMEV} ${LIBRARY}
	@${LINK} ${SONAMEV} ${SONAME}
	@${CHMOD} 755 ${SONAMEV}
	@${ECHO} "Static Library Archive:"
	$(Q)${LS}   ${ARCHIVE}
	$(Q)${SZ}   ${ARCHIVE}
	@${ECHO} "Shared Object Library:"
	$(Q)${LS}   ${SONAMEV}
	$(Q)${SZ}   ${SONAMEV}
	$(Q)${FILE} ${SONAMEV}

# *************************************************************************** #
# The .depend file contains the header file dependencies.                     #
# *************************************************************************** #

-include .depend

# end of Makefile
