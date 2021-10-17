#
# Makefile generated by:
# codebench 0.55
#
# Project: MediaVault
#
# Created on: 14-10-2021 23:58:06
#
#

###################################################################
##
##////  Objects
##
###################################################################

MediaVault_OBJ := \
	 src/mediavault.o src/libshandler.o src/gui.o \
	 src/mainWin.o src/guifuncs.o src/aboutWin.o \
	 src/httpfuncs.o src/radiofuncs.o src/stringfuncs.o \
	 src/fsfuncs.o src/podcastfuncs.o src/sqldb.o \
	


###################################################################
##
##////  Variables and Environment
##
###################################################################

CC := gcc:bin/gcc

INCPATH := -I.

CFLAGS := $(INCPATH) -Wall -Werror -Wwrite-strings -Wextra -O3


###################################################################
##
##////  General rules
##
###################################################################

.PHONY: all all-before all-after clean clean-custom realclean

all: all-before MediaVault all-after

all-before:
#	You can add rules here to execute before the project is built

all-after:
#	You can add rules here to execute after the project is built

clean: clean-custom
	@echo "Cleaning compiler objects..."
	@rm -f  $(MediaVault_OBJ)

realclean:
	@echo "Cleaning compiler objects and targets..."
	@rm -f  $(MediaVault_OBJ) MediaVault


###################################################################
##
##////  Targets
##
###################################################################

MediaVault: $(MediaVault_OBJ)
	@echo "Linking MediaVault"
	@gcc:bin/gcc -o MediaVault $(MediaVault_OBJ) -lsqlite3 -lcurl -lrtmp -lssl -lcrypto -lz -lpthread
	@echo "Removing stale debug target: MediaVault"
	@rm -f MediaVault.debug


###################################################################
##
##////  Standard rules
##
###################################################################

# A default rule to make all the objects listed below
# because we are hiding compiler commands from the output

.c.o:
	@echo "Compiling $<"
	@$(CC) -c $< -o $*.o $(CFLAGS)

src/mediavault.o: src/mediavault.c src/libshandler.h src/gui.h \
	 src/sqldb.h src/mediavault.h src/globals.h \
	

src/libshandler.o: src/libshandler.c src/globals.h src/version.h \
	

src/upnpfuncs.o: src/upnpfuncs.c src/upnpfuncs.h src/globals.h \
	

src/gui.o: src/gui.c src/globals.h src/version.h \
	 src/gui.h src/guifuncs.h src/mainWin.h \
	 src/aboutWin.h src/radiofuncs.h src/podcastfuncs.h \
	

src/httpfuncs.o: src/httpfuncs.c src/version.h

src/fsfuncs.o: src/fsfuncs.c src/globals.h src/version.h \
	

src/sqldb.o: src/sqldb.c src/version.h

src/guifuncs.o: src/guifuncs.c src/globals.h src/version.h \
	 src/gui.h src/guifuncs.h src/radiofuncs.h \
	 src/httpfuncs.h src/stringfuncs.h

src/podcastfuncs.o: src/podcastfuncs.c src/globals.h src/version.h \
	 src/gui.h src/libshandler.h src/httpfuncs.h \
	 src/stringfuncs.h src/guifuncs.h src/secrets.h \
	

src/mainWin.o: src/mainWin.c src/globals.h src/version.h \
	 src/gui.h src/mainWin.h

src/aboutWin.o: src/aboutWin.c src/globals.h src/version.h \
	 src/gui.h

src/radiofuncs.o: src/radiofuncs.c src/globals.h src/version.h \
	 src/gui.h src/libshandler.h src/httpfuncs.h \
	 src/stringfuncs.h src/guifuncs.h

src/stringfuncs.o: src/stringfuncs.c src/globals.h src/version.h \
	 src/libshandler.h


###################################################################
##
##////  Custom rules
##
###################################################################

release:
	mkdir -p release/MediaVault/libs
	mkdir -p release/MediaVault/cache
	mkdir -p release/MediaVault/scripts
	cp release_files/* release/MediaVault/ -r
	cp MediaVault release/MediaVault/
	strip release/MediaVault/MediaVault
	cp CHANGELOG.md release/MediaVault/
	cp README.md release/MediaVault/
	cp LICENSE release/MediaVault/
	cp COPYING release/MediaVault/
	cp images release/MediaVault/ -r
	cp scripts/*.sample* release/MediaVault/scripts/
	cp Libs:jansson.library release/MediaVault/libs/
	cp release_files/Icons/IconDesigner.info release/MediaVault.info
	lha -aeqr3 a MediaVault.lha release/


###################################################################
