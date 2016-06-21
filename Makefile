###########################################################################
# Makefile for Thermostat
###########################################################################

# Used for internal functionality macros. Feel free to rename. Need to
#   replace this with an autoconf script which I haven't yet learned how to
#   write.

OPTIMIZATION       = -O0 -g
C_STANDARD         = gnu99
CPP_STANDARD       = gnu++11


# Environmental awareness...
###########################################################################
SHELL        = /bin/sh
WHO_I_AM     = $(shell whoami)
WHERE_I_AM   = $(shell pwd)

export CXX   = $(shell which g++)
export CC    = $(shell which gcc)

export OUTPUT_PATH = $(WHERE_I_AM)/


###########################################################################
# Source files, includes, and linker directives...
###########################################################################
INCLUDES    = -I$(WHERE_I_AM)/. -I./resource/include/
INCLUDES   += -I./resource/csdk/stack/include/
INCLUDES   += -I./resource/csdk/security/include/
INCLUDES   += -I./resource/csdk/routing/include/
INCLUDES   += -I./resource/csdk/logger/include/
INCLUDES   += -I./resource/c_common/
INCLUDES   += -I./resource/c_common/ocrandom/include/
INCLUDES   += -I./resource/c_common/oic_time/include/
INCLUDES   += -I./resource/c_common/oic_string/include/
INCLUDES   += -I./resource/c_common/oic_malloc/include/
INCLUDES   += -I./resource/oc_logger/include/
INCLUDES   += -I./resource/oic/include/
INCLUDES   += -I./service/resource-encapsulation/include/


# This is awful. Sick of fighting with IoTivities constant and unpredictable
# build-system failure. Until they fix their SDK installation, we are forced
#   to do this...
IOTIVITY_LOCUS = $(WHERE_I_AM)/out/linux/x86_64/release/

# Libraries to link
LIBS  = -L$(IOTIVITY_LOCUS) -lstdc++ -lm -lpthread -ldl
LIBS += -lc_common
#LIBS += -llogger
#LIBS += -lcoap
#LIBS += -locsrm
LIBS += -loctbstack
LIBS += -lrcs_common
#LIBS += -lrcs_container
LIBS += -lrcs_server
#LIBS += -lrcs_client
#LIBS += -ltinydtls
#LIBS += -lresource_hosting -ltimer
#LIBS += -lplugin_interface
#LIBS += -lconnectivity_abstraction
#LIBS += -lroutingmanager
#LIBS += -lscene_manager
#LIBS += -lTGMSDKLibrary
#LIBS += -lYamlParser

# These are for shared library builds...
LIBS += -loc_logger
LIBS += -loc


# Wrap the include paths into the flags...
CFLAGS  = $(OPTIMIZATION) -Wall $(INCLUDES) -DSECURED=1 -DTARGET_OS=linux
CFLAGS += -Wdouble-promotion -frtti -fexceptions


###########################################################################
# Source file definitions...
###########################################################################
SRCS   = Thermostat.cpp

export CFLAGS
export CPP_FLAGS = $(CFLAGS)


###########################################################################
# Rules for building the Thermostat follow...
###########################################################################

.PHONY: all


all: clean
	$(CXX) -o Thermostat $(SRCS) $(CFLAGS) -std=$(CPP_STANDARD) $(LIBS) -D_GNU_SOURCE -O2

clean:
	rm -f *.o *.su Thermostat
