#
# Makefile generated by:
# codebench 0.55
#
# Project: MediaVault
#
# Created on: 07-04-2021 18:41:34
#
#

###################################################################
##
##////  Objects
##
###################################################################

MediaVault_OBJ := \
	 src/mediavault.o src/libshandler.o src/upnpfuncs.o \
	 src/gui.o


###################################################################
##
##////  Variables and Environment
##
###################################################################

CC := gcc:bin/gcc

INCPATH := -I.

CFLAGS := $(INCPATH) -Wall -Werror -Wwrite-strings


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
	@gcc:bin/gcc -o MediaVault $(MediaVault_OBJ) -lauto
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

src/mediavault.o: src/mediavault.c src/mediavault.h

src/libshandler.o: src/libshandler.c src/libshandler.h

src/upnpfuncs.o: src/upnpfuncs.c src/upnpfuncs.h

src/oofuncs.o: src/oofuncs.c

src/gui.o: src/gui.c src/gui.h

