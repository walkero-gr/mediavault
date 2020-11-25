##########################################################################
# Compiler settings
##########################################################################
CC			= vc
LINK		= vc
# INCLUDES	= -I$(NDK_INC) -I$(MUI38_INC)
INCLUDES_OS4= -I$(AOS4_NLIB_INC) -I$(AOS4_SDK_INC) -I$(AOS4_NET_INC) -v
# INCLUDES_MOS= -I$(NDK_INC) -I$(MUI50_INC)

DATEISO = $(shell date --iso=date)
DATESTR = $(shell date "+%Y%m%d")

# DRONE_TAG is set by Drone CI/CD
# Parse the repo tag to different defines, that will be used while
# compiling MediaVault
#
# The tags should be like v(MAJOR).(MINOR) or v(MAJOR).(MINOR)b(BETA)
# in example v1.1 or v1.0b5
#
ifneq ($(origin DRONE_TAG),undefined)
	MAJOR = $(patsubst v%,%,$(firstword $(subst ., ,$(DRONE_TAG))))

	ifeq ($(findstring b,$(DRONE_TAG)), b)
		MINOR = $(lastword $(subst ., , $(firstword $(subst b, ,$(DRONE_TAG)))))
		BETA = $(lastword $(subst b, ,$(DRONE_TAG)))

		VERS_FLAGS = -DMAJOR_VERS=$(MAJOR) -DMINOR_VERS=$(MINOR) -DBETA_VERS=$(BETA) -DRELEASE_DATE=$(DATEISO)
	else
		MINOR = $(lastword $(subst ., ,$(DRONE_TAG)))

		VERS_FLAGS = -DMAJOR_VERS=$(MAJOR) -DMINOR_VERS=$(MINOR) -DRELEASE_DATE=$(DATEISO)
	endif
else
	VERS_FLAGS = -DRELEASE_DATE=$(DATEISO)
endif

# CFLAGS		= -c +aos68k -dontwarn=-1 -O2 -c99 -DCPU_VERS=68000 $(VERS_FLAGS)
# CFLAGS_MOS	= -c +morphos -dontwarn=-1 -O2 -DCPU_VERS=MorphOS $(VERS_FLAGS)
CFLAGS_OS4	= -c +aosppc -dontwarn=-1 -O2 -D__USE_INLINE__ $(VERS_FLAGS)

##########################################################################
# Builder settings
##########################################################################
#MKLIB			= join
# LIBFLAGS		= +aos68k -lamiga -lauto -o
# LIBFLAGS_MOS	= +morphos -lamiga -lauto -o
LIBFLAGS_OS4	= +aosppc -lamiga -lauto -o

##########################################################################
# Object files
##########################################################################
OBJS_OS4	= src/MediaVault_OS4.o


.PHONY: MediaVault.OS4

default: MediaVault.OS4

##########################################################################
# Rule for building
##########################################################################
MediaVault.OS4: $(OBJS_OS4)
	$(LINK) $(OBJS_OS4) $(LIBFLAGS_OS4) $@

##########################################################################
# object files (AOS4)
##########################################################################

src/MediaVault_OS4.o: src/mediavault.c
	$(CC) $(CFLAGS_OS4) $(INCLUDES_OS4) -o $@ src/mediavault.c

##########################################################################
# generic build options
##########################################################################

clean:
	rm MediaVault.* src/*.o

release:
	# TBD

clean-release:
	# TBD
