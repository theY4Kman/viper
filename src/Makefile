# Viper Makefile
#   Copyright (C) 2007-2009 Zach Kanzler
#   Copyright (C) 2004-2009 SourceMod Development Team
# Makefile originally written by David "BAILOPAN" Anderson

SMSDK = ../../sourcemod-central
SRCDS_BASE = ~/srcds
HL2SDK_ORIG = ../../hl2sdk
HL2SDK_OB = ../../hl2sdk-ob
HL2SDK_L4D = ../../hl2sdk-l4d
MMSOURCE17 = ../../mmsource-1.7

PYTHON = /home/they4kman/apps/Python-2.5.4

#####################################
### EDIT BELOW FOR OTHER PROJECTS ###
#####################################

PROJECT = viper

USEMETA = true

OBJECTS += sdk/smsdk_ext.cpp extension.cpp viper.cpp console.cpp sourcemod/sm_trie.cpp

OBJECTS += systems/ConCmdManager.cpp systems/ConVarManager.cpp systems/EventManager.cpp \
           systems/PlayerManager.cpp systems/PluginSys.cpp systems/ForwardSys.cpp

OBJECTS += python/py_clients.cpp python/py_events.cpp python/py_console.cpp \
           python/py_forwards.cpp python/py_sourcemod.cpp python/py_entity.cpp \
           python/py_halflife.cpp python/py_keyvalues.cpp python/py_datatypes.cpp

##############################################
### CONFIGURE ANY OTHER FLAGS/OPTIONS HERE ###
##############################################

C_OPT_FLAGS = -DNDEBUG -O3 -funroll-loops -pipe -fno-strict-aliasing -fno-threadsafe-statics
C_DEBUG_FLAGS = -D_DEBUG -DDEBUG -g -ggdb3
C_GCC4_FLAGS = -fvisibility=hidden
CPP_GCC4_FLAGS = -fvisibility-inlines-hidden
CPP = gcc-4.1

override ENGSET = false
ifeq "$(ENGINE)" "original"
	HL2SDK = $(HL2SDK_ORIG)
	HL2PUB = $(HL2SDK)/public
	HL2LIB = $(HL2SDK)/linux_sdk
	CFLAGS += -DSOURCE_ENGINE=1
	METAMOD = $(MMSOURCE17)/core-legacy
	INCLUDE += -I$(HL2SDK)/public/dlls
	SRCDS = $(SRCDS_BASE)
	override ENGSET = true
endif
ifeq "$(ENGINE)" "orangebox"
	HL2SDK = $(HL2SDK_OB)
	HL2PUB = $(HL2SDK)/public
	HL2LIB = $(HL2SDK)/lib/linux
	CFLAGS += -DSOURCE_ENGINE=3
	METAMOD = $(MMSOURCE17)/core
	INCLUDE += -I$(HL2SDK)/public/game/server
	SRCDS = $(SRCDS_BASE)/orangebox
	override ENGSET = true
endif
ifeq "$(ENGINE)" "left4dead"
	HL2SDK = $(HL2SDK_L4D)
	HL2PUB = $(HL2SDK)/public
	HL2LIB = $(HL2SDK)/lib/linux
	CFLAGS += -DSOURCE_ENGINE=4
	METAMOD = $(MMSOURCE17)/core
	INCLUDE += -I$(HL2SDK)/public/game/server
	SRCDS = $(SRCDS_BASE)/l4d
	override ENGSET = true
endif

CFLAGS += -DSE_EPISODEONE=1 -DSE_DARKMESSIAH=2 -DSE_ORANGEBOX=3 -DSE_LEFT4DEAD=4

ifeq "$(USEMETA)" "true"
	LINK_HL2 = $(HL2LIB)/tier1_i486.a vstdlib_i486.so tier0_i486.so

	LINK += $(LINK_HL2)

	INCLUDE += -I. -I.. -Isdk -I$(HL2PUB) -I$(HL2PUB)/engine -I$(HL2PUB)/tier0 -I$(HL2PUB)/tier1 \
		-I$(HL2PUB)/vstdlib \
		-I$(METAMOD) -I$(METAMOD)/sourcehook -I$(SMSDK)/public -I$(SMSDK)/public/sourcepawn
else
	INCLUDE += -I. -I.. -Isdk -I$(SMSDK)/public -I$(SMSDK)/public/sourcepawn
endif

LINK += -m32 -lm -ldl -Wl,--rpath,\$$ORIGIN/../viper/lib/plat-linux2 libpython2.5.so

INCLUDE += -I../public -Isystems -Isourcemod -I$(PYTHON) -I$(PYTHON)/Include

CFLAGS += -D_LINUX -Dstricmp=strcasecmp -D_stricmp=strcasecmp -D_strnicmp=strncasecmp -Dstrnicmp=strncasecmp \
	-D_snprintf=snprintf -D_vsnprintf=vsnprintf -D_alloca=alloca -Dstrcmpi=strcasecmp -Wall -Werror -Wno-switch \
	-Wno-unused -Wno-uninitialized -mfpmath=sse -msse -DSOURCEMOD_BUILD -DHAVE_STDINT_H -m32
CPPFLAGS += -Wno-non-virtual-dtor -fno-exceptions -fno-rtti

################################################
### DO NOT EDIT BELOW HERE FOR MOST PROJECTS ###
################################################

ifeq "$(DEBUG)" "true"
	BIN_DIR = build/debug
	CFLAGS += $(C_DEBUG_FLAGS)
else
	BIN_DIR = build/release
	CFLAGS += $(C_OPT_FLAGS)
endif

ifeq "$(USEMETA)" "true"
	BIN_DIR := $(BIN_DIR).$(ENGINE)
endif

OS := $(shell uname -s)
ifeq "$(OS)" "Darwin"
	LINK += -dynamiclib
	BINARY = $(PROJECT).ext.dylib
else
	LINK += -static-libgcc -shared
	BINARY = $(PROJECT).ext.so
endif

GCC_VERSION := $(shell $(CPP) -dumpversion >&1 | cut -b1)
ifeq "$(GCC_VERSION)" "4"
	CFLAGS += $(C_GCC4_FLAGS)
	CPPFLAGS += $(CPP_GCC4_FLAGS)
endif

OBJ_LINUX := $(OBJECTS:%.cpp=$(BIN_DIR)/%.o)

$(BIN_DIR)/%.o: %.cpp
	$(CPP) $(INCLUDE) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<

all: check
	mkdir -p $(BIN_DIR)/sdk
	mkdir -p $(BIN_DIR)/systems
	mkdir -p $(BIN_DIR)/python
	mkdir -p $(BIN_DIR)/sourcemod
	ln -sf $(SRCDS)/bin/vstdlib_i486.so vstdlib_i486.so
	ln -sf $(SRCDS)/bin/tier0_i486.so tier0_i486.so
#	ln -sf build/addons/sourcemod/extensions/viper/lib/plat-linux2/libpython2.5.so.1.0 libpython2.5.so
	ln -sf $(SRCDS)/cstrike/addons/sourcemod/extensions/viper/lib/plat-linux2/libpython2.5.so.1.0 libpython2.5.so
	$(MAKE) -f Makefile extension

check:
	if [ "$(USEMETA)" = "true" ] && [ "$(ENGSET)" = "false" ]; then \
		echo "You must supply ENGINE=left4dead, ENGINE=orangebox, or ENGINE=original"; \
		exit 1; \
	fi

extension: check $(OBJ_LINUX)
	$(CPP) $(INCLUDE) $(OBJ_LINUX) $(LINK) -o $(BIN_DIR)/$(BINARY)

debug:
	$(MAKE) -f Makefile all DEBUG=true

default: all

clean: check
	rm -rf $(BIN_DIR)/*.o
	rm -rf $(BIN_DIR)/sdk/*.o
	rm -rf $(BIN_DIR)/systems/*.o
	rm -rf $(BIN_DIR)/python/*.o
	rm -rf $(BIN_DIR)/sourcemod/*.o
	rm -rf $(BIN_DIR)/$(BINARY)

install:
	if [ "$(ENGINE)" = "orangebox" ]; then \
	    cp -R $(BIN_DIR)/$(BINARY) $(SRCDS_BASE)/cstrike/addons/sourcemod/extensions/auto.2.ep2/; \
	else if [ "$(ENGINE)" = "original" ]; then \
	    cp -R $(BIN_DIR)/$(BINARY) $(SRCDS_BASE)/cstrike/addons/sourcemod/extensions/auto.1.ep1/; \
	else if [ "$(ENGINE)" = "left4dead" ]; then \
	    cp -R $(BIN_DIR)/$(BINARY) $(SRCDS_BASE)/cstrike/addons/sourcemod/extensions/auto.2.l4d/; \
	fi; fi; fi

