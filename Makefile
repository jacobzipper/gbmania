# ################################################################################
# # These are variables for the GBA toolchain build
# # You can add others if you wish to
# # ***** Jacob Zipper *****
# ################################################################################

# # TA-TODO: Put your game name here.
# # The name of your desired application
# # This should be a just a name i.e MyFirstGBAGame
# # No SPACES AFTER THE NAME.
# PROGNAME = Game

# # TA-TODO: Add the C files you want compiled here (replace extension with .o)
# # Here you must put a list of all of the object files
# # that will be compiled into your program. For example
# # if you have main.c and myLib.c then in the following
# # line you would put main.o and myLib.o
# OFILES = userlibs/gfx.o audio/nbgtest.o images/garbage.o main.o

# export LIBRARY_PATH = ./libs

# ################################################################################
# # These are various settings used to make the GBA toolchain work
# # DO NOT EDIT BELOW.
# ################################################################################

# .PHONY: all
# all : CFLAGS += $(CRELEASE) -I../shared
# all : LDFLAGS += $(LDRELEASE)
# all: $(PROGNAME).gba
# 	@echo "[FINISH] Created $(PROGNAME).gba"

# include /opt/cs2110-tools/GBAVariables.mak
# include ./tonc_rules

# # Son, it's time to be an adult and use adult prototypes
# CFLAGS += -Wstrict-prototypes -Wold-style-definition -std=c11

# debug : CFLAGS += $(CDEBUG) -I../shared
# debug : LDFLAGS += $(LDDEBUG)
# debug : $(PROGNAME).gba
# 	@echo "[FINISH] Created $(PROGNAME).gba"

# .PHONY : emu
# emu : med

# .PHONY : vba
# vba : CFLAGS += $(CRELEASE) -I../shared
# vba : LDFLAGS += $(LDRELEASE)
# vba : $(PROGNAME).gba
# 	@echo "[EXECUTE] Running Emulator VBA-M"
# 	@echo "          Please see emulator.log if this fails"
# 	@gvbam $(VBAOPT) $(PROGNAME).gba >emulator.log 2>&1

# .PHONY : med
# med : CFLAGS += $(CRELEASE) -I../shared
# med : LDFLAGS += $(LDRELEASE)
# med : $(PROGNAME).gba
# 	@echo "[EXECUTE] Running emulator Mednafen"
# 	@echo "          Please see emulator.log if this fails"
# # Default mednafen keybinds are "Gaaarbage," in the words of Farzam
# 	@mkdir -p ~/.mednafen/
# 	@cp mednafen-09x.cfg ~/.mednafen/
# 	@mednafen $(MEDOPT) $(PROGNAME).gba >emulator.log 2>&1

# .PHONY : submit
# submit: clean
# 	@rm -f submission.tar.gz
# 	@tar czvf submission.tar.gz $(wildcard *)

# .PHONY : clean
# clean :
# 	@echo "[CLEAN] Removing all compiled files"
# 	rm -f *.o *.elf *.gba *.log */*.o



#
# Template tonc makefile
#
# Yoinked mostly from DKP's template
#

# === SETUP ===========================================================

# --- No implicit rules ---
.SUFFIXES:

# --- Tonc paths ---
# If not defined as environment variable, assumed to be 2 dirs up
export TONCCODE	?= $(CURDIR)

include $(TONCCODE)/tonc_rules

# --- Main path ---

PATH  := $(PATH):$(PWD)/bin
SHELL := env PATH=$(PATH) /bin/bash

# === PROJECT DETAILS =================================================
# PROJ		: Base project name
# TITLE		: Title for ROM header (12 characters)
# LIBS		: Libraries to use, formatted as list for linker flags
# BUILD		: Directory for build process temporaries. Should NOT be empty!
# SRCDIRS	: List of source file directories
# DATADIRS	: List of data file directories
# INCDIRS	: List of header file directories
# LIBDIRS	: List of library directories
# General note: use `.' for the current dir, don't leave the lists empty.

export PROJ	?= $(notdir $(CURDIR))
TITLE		:= $(PROJ)

LIBS		:= -lmm -lgba

BUILD		:= build
SRCDIRS		:= source
DATADIRS	:=
INCDIRS		:= include
LIBDIRS		:= $(CURDIR)
MUSIC		:= audio

# --- switches ---

bMB		:= 0	# Multiboot build
bTEMPS	:= 0	# Save gcc temporaries (.i and .s files)
bDEBUG2	:= 0	# Generate debug info (bDEBUG2? Not a full DEBUG flag. Yet)

# === BUILD FLAGS =====================================================
# This is probably where you can stop editing
# NOTE: I've noticed that -fgcse and -ftree-loop-optimize sometimes muck
#	up things (gcse seems fond of building masks inside a loop instead of
#	outside them for example). Removing them sometimes helps

# --- Architecture ---

ARCH    := -mthumb-interwork -mthumb
RARCH   := -mthumb-interwork -mthumb
IARCH   := -mthumb-interwork -marm -mlong-calls

GCC_VERSION = $(shell arm-none-eabi-gcc -dumpversion)
LINKSCRIPT_DIR = /opt/cs2110-tools

ARMINC = /usr/arm-none-eabi/include
ARMLIB = /usr/arm-none-eabi/lib
GCCLIB = /usr/lib/gcc/arm-none-eabi/$(GCC_VERSION)


# --- Main flags ---

CFLAGS		:= -mcpu=arm7tdmi -mtune=arm7tdmi -std=c99 -Wall -pedantic -Wextra -Werror -O2 -Wstrict-prototypes -Wold-style-definition
CFLAGS		+= $(INCLUDE)
CFLAGS		+= -ffast-math -fno-strict-aliasing

CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions

ASFLAGS		:= $(ARCH) $(INCLUDE)
LDFLAGS 	:= $(ARCH) -Wl,-Map,$(PROJ).map \
 -lc -lgcc -L $(ARMLIB) \
          -L $(ARMLIB)/thumb \
          -L $(GCCLIB)

# --- Multiboot ? ---
ifeq ($(strip $(bMB)), 1)
	TARGET	:= $(PROJ).mb
else
	TARGET	:= $(PROJ)
endif

# --- Save temporary files ? ---
ifeq ($(strip $(bTEMPS)), 1)
	CFLAGS		+= -save-temps
	CXXFLAGS	+= -save-temps
endif

# --- Debug info ? ---

ifeq ($(strip $(bDEBUG)), 1)
	CFLAGS		+= -DDEBUG -g
	CXXFLAGS	+= -DDEBUG -g
	ASFLAGS		+= -DDEBUG -g
	LDFLAGS		+= -g
else
	CFLAGS		+= -DNDEBUG
	CXXFLAGS	+= -DNDEBUG
	ASFLAGS		+= -DNDEBUG
endif

# === BUILD PROC ======================================================

ifneq ($(BUILD),$(notdir $(CURDIR)))

# Still in main dir:
# * Define/export some extra variables
# * Invoke this file again from the build dir
# PONDER: what happens if BUILD == "" ?

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export VPATH	:=									\
	$(foreach dir, $(SRCDIRS) , $(CURDIR)/$(dir))	\
	$(foreach dir, $(DATADIRS), $(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

# --- List source and data files ---

CFILES		:=	$(foreach dir, $(SRCDIRS) , $(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir, $(SRCDIRS) , $(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir, $(SRCDIRS) , $(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir, $(DATADIRS), $(notdir $(wildcard $(dir)/*.*)))

# --- switched additions ----------------------------------------------

ifneq ($(strip $(MUSIC)),)
	export AUDIOFILES	:=	$(foreach dir,$(notdir $(wildcard $(MUSIC)/*.*)),$(CURDIR)/$(MUSIC)/$(dir))
	BINFILES += soundbank.bin
endif

# --- Set linker depending on C++ file existence ---
ifeq ($(strip $(CPPFILES)),)
	export LD	:= $(CC)
else
	export LD	:= $(CXX)
endif

# --- Define object file list ---
export OFILES_BIN := $(addsuffix .o,$(BINFILES))

export OFILES_SOURCES := $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export OFILES	:=	$(addsuffix .o, $(BINFILES))					\
					$(CFILES:.c=.o) $(CPPFILES:.cpp=.o)				\
					$(SFILES:.s=.o)

# --- Create include and library search paths ---
export HFILES := $(addsuffix .h,$(subst .,_,$(BINFILES)))

export INCLUDE	:=	$(foreach dir,$(INCDIRS),-I$(CURDIR)/$(dir))	\
					$(foreach dir,$(LIBDIRS),-I$(dir)/include)		\
					-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	-L$(CURDIR) $(foreach dir,$(LIBDIRS),-L$(dir)/lib)

# --- More targets ----------------------------------------------------

.PHONY: $(BUILD) clean

# --- Create BUILD if necessary, and run this makefile from there ---

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@chmod a+rwx $(CURDIR)/bin/*
	@make --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
	@arm-none-eabi-nm -Sn $(OUTPUT).elf > $(BUILD)/$(TARGET).map


all	: $(BUILD)

clean:
	@echo clean ...
	@rm -fr $(BUILD) $(OUTPUT).elf $(OUTPUT).gba *.gz *.o

.PHONY : emu
emu : med

.PHONY : vba
vba : $(BUILD)
	@echo "[EXECUTE] Running Emulator VBA-M"
	@echo "          Please see emulator.log if this fails"
	@vbam $(VBAOPT) $(OUTPUT).gba >emulator.log 2>&1

.PHONY : gvba
gvba : $(BUILD)
	@echo "[EXECUTE] Running Emulator VBA-M"
	@echo "          Please see emulator.log if this fails"
	@gvbam $(VBAOPT) $(OUTPUT).gba >emulator.log 2>&1

.PHONY : med
med : $(BUILD)
	@echo "[EXECUTE] Running emulator Mednafen"
	@echo "          Please see emulator.log if this fails"
# Default mednafen keybinds are "Gaaarbage," in the words of Farzam
	@mkdir -p ~/.mednafen/
	@cp mednafen-09x.cfg ~/.mednafen/
	@mednafen $(MEDOPT) $(OUTPUT).gba >emulator.log 2>&1



else		# If we're here, we should be in the BUILD dir

DEPENDS	:=	$(BUILD)/*.d

# --- Main targets ----

$(OUTPUT).gba	:	$(OUTPUT).elf

$(OUTPUT).elf	:	$(OFILES)

$(OFILES_SOURCES) : $(HFILES)


#---------------------------------------------------------------------------------
# The bin2o rule should be copied and modified
# for each extension used in the data directories
#---------------------------------------------------------------------------------

#---------------------------------------------------------------------------------
# rule to build soundbank from music files
#---------------------------------------------------------------------------------
soundbank.bin soundbank.h : $(AUDIOFILES)
#---------------------------------------------------------------------------------
	@mmutil $^ -osoundbank.bin -hsoundbank.h

#---------------------------------------------------------------------------------
# This rule links in binary data with the .bin extension
#---------------------------------------------------------------------------------
%.bin.o	%_bin.h :	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)
-include $(DEPPSDIR)/*.d



endif		# End BUILD switch

.PHONY : submit
submit: clean
	@rm -f submission.tar.gz
	@tar czvf submission.tar.gz $(wildcard *)

# EOF
