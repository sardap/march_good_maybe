#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/gba_rules

LIBTONC := $(DEVKITPRO)/tonclib $(DEVKITPRO)/libtonc

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
# DATA is a list of directories containing binary data
# GRAPHICS is a list of directories containing files to be processed by grit
#
# All directories are specified relative to the project directory where
# the makefile is found
#
#---------------------------------------------------------------------------------
TARGET		:= $(notdir $(CURDIR))
BUILD		:= build
SOURCES		:= source source/scenes source/systems build/gfx 
INCLUDES	:= include 
DATA		:=
MUSIC		:=

CPP_VERSION = 20

ifndef OPTIMIZATION_LEVEL
	OPTIMIZATION_LEVEL = O0
endif

IGNORE_WARNINGS = -Wno-volatile
ENTT_HEADER_FILE_DOWNLOAD_LINK = https://raw.githubusercontent.com/skypjack/entt/e5172a9240728cc271af7599c6f13580329f3618/single_include/entt/entt.hpp
MGBA_HEADER_FILE_DOWNLOAD_LINK = https://raw.githubusercontent.com/mgba-emu/mgba/686eee4e20cc555b90810811f2d0d6f8b9b393c5/opt/libgba/mgba.h
MGBA_SOURCE_FILE_DOWNLOAD_LINK = https://raw.githubusercontent.com/mgba-emu/mgba/686eee4e20cc555b90810811f2d0d6f8b9b393c5/opt/libgba/mgba.c


#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-mthumb -mthumb-interwork

CFLAGS	:=	-g -Wall -$(OPTIMIZATION_LEVEL)\
		-mcpu=arm7tdmi -mtune=arm7tdmi\
		$(IGNORE_WARNINGS)\
		$(ARCH)

CFLAGS	+=	$(INCLUDE) 

CXXFLAGS	:=	$(CFLAGS) -std=c++$(CPP_VERSION)\
	-fno-rtti\
	-fno-exceptions\


ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-g $(ARCH) -Wl,-Map,$(notdir $*.map)

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:= -lmm -lgba -ltonc


#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:=	$(LIBGBA) $(LIBTONC)

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------


ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir)) \
			$(foreach dir,$(GRAPHICS),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

ifneq ($(strip $(MUSIC)),)
	export AUDIOFILES	:=	$(foreach dir,$(notdir $(wildcard $(MUSIC)/*.*)),$(CURDIR)/$(MUSIC)/$(dir))
	BINFILES += soundbank.bin
endif

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES_BIN := $(addsuffix .o,$(BINFILES))

export OFILES_SOURCES := $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export OFILES := $(OFILES_BIN) $(OFILES_SOURCES)

export HFILES := $(addsuffix .h,$(subst .,_,$(BINFILES)))

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-iquote $(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: $(BUILD) clean

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
ifeq (,$(wildcard ./build/entt/entt.hpp))
	@mkdir -p build/entt
	@wget -O build/entt/entt.hpp $(ENTT_HEADER_FILE_DOWNLOAD_LINK)
endif
ifeq (,$(wildcard ./build/mgba/mgba.h))
	@mkdir -p build/mgba
	@wget -O build/mgba/mgba.h $(MGBA_HEADER_FILE_DOWNLOAD_LINK)
endif
ifeq (,$(wildcard ./build/mgba/mgba.c))
	@mkdir -p build/mgba
	@wget -O build/mgba/mgba.c $(MGBA_SOURCE_FILE_DOWNLOAD_LINK)
endif
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).elf $(TARGET).gba


#---------------------------------------------------------------------------------
else

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------

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


-include $(DEPSDIR)/*.d
#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
