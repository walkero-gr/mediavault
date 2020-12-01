#
# Makefile generated by:
# codebench 0.55
#
# Project: MediaVault
#
# Created on: 01-12-2020 17:35:51
#
#

###################################################################
##
##////  Objects
##
###################################################################

MediaVault.OS4_OBJ := \
	 src/mediavault.o


###################################################################
##
##////  Variables and Environment
##
###################################################################

CC := gcc:bin/gcc

INCPATH := -I.

CFLAGS := $(INCPATH) -gstabs -Wall -Werror -Wwrite-strings -D__USE_INLINE__


###################################################################
##
##////  General rules
##
###################################################################

.PHONY: all all-before all-after clean clean-custom realclean

all: all-before MediaVault.OS4 all-after

all-before:
#	You can add rules here to execute before the project is built

all-after:
#	You can add rules here to execute after the project is built

clean: clean-custom
	@echo "Cleaning compiler objects..."
	@rm -f  $(MediaVault.OS4_OBJ)

realclean:
	@echo "Cleaning compiler objects and targets..."
	@rm -f  $(MediaVault.OS4_OBJ) MediaVault.OS4


###################################################################
##
##////  Targets
##
###################################################################

MediaVault.OS4: $(MediaVault.OS4_OBJ)
	@echo "Linking MediaVault.OS4.debug"
	@gcc:bin/gcc -o MediaVault.OS4.debug $(MediaVault.OS4_OBJ) -lauto
	@echo "Creating debug target: MediaVault.OS4"
	@cp -f -p MediaVault.OS4.debug MediaVault.OS4


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

src/mediavault.o: src/mediavault.c

