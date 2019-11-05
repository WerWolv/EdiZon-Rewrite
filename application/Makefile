#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

TOPDIR ?= $(CURDIR)
include $(TOPDIR)/../switch_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# DATA is a list of directories containing data files
# INCLUDES is a list of directories containing header files
# ROMFS is the directory containing data to be added to RomFS, relative to the Makefile (Optional)
#
# NO_ICON: if set to anything, do not use icon.
# NO_NACP: if set to anything, no .nacp file is generated.
# APP_TITLE is the name of the app stored in the .nacp file (Optional)
# APP_AUTHOR is the author of the app stored in the .nacp file (Optional)
# APP_VERSION is the version of the app stored in the .nacp file (Optional)
# APP_TITLEID is the titleID of the app stored in the .nacp file (Optional)
# ICON is the filename of the icon (.jpg), relative to the project folder.
#   If not set, it attempts to use one of the following (in this order):
#     - <Project name>.jpg
#     - icon.jpg
#     - <libnx folder>/default_icon.jpg
#
# CONFIG_JSON is the filename of the NPDM config file (.json), relative to the project folder.
#   If not set, it attempts to use one of the following (in this order):
#     - <Project name>.json
#     - config.json
#   If a JSON file is provided or autodetected, an ExeFS PFS0 (.nsp) is built instead
#   of a homebrew executable (.nro). This is intended to be used for sysmodules.
#   NACP building is skipped as well.
#---------------------------------------------------------------------------------
VERSION_MAJOR := 4
VERSION_MINOR := 0
VERSION_MICRO := 0
NIGHTLY		  := 1

APP_TITLE	:=	EdiZon
APP_AUTHOR	:=	WerWolv

ifeq ($(NIGHTLY), 1)
	APP_VERSION	:=	${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO} - Nightly
else
	APP_VERSION	:=	${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
endif

TARGET			:=	EdiZon
OUTDIR			:=	out
BUILD			:=	build
SOURCES_TOP		:=	source ../common/source libs/lua libs/zipper/zipper libs/zipper/minizip
SOURCES			+=  $(filter-out $(EXT_INCLUDES),$(foreach dir,$(SOURCES_TOP),$(shell find $(dir) -type d 2>/dev/null)))
INCLUDES		:=	include ../common/include libs/json/include libs/libpython/include libs/libpython/include/python libs/lua libs/mruby/include libs/zipper/zipper libs/zipper/minizip libs/quickjs
EXT_INCLUDES	:=  libs/zipper/minizip/aes libs/lua/testes/libs
EXCLUDE_FOLDERS := 
EXCLUDES		:=	onelua.c lua.c iowin32.c minizip.c miniunz.c dirent.c 
DATA			:=	data
ROMFS			:=	romfs

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH		:=	-march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE

null      	:=
SPACE     	:=  $(null) $(null)

DEFINES 	+=  -Dfopen64=fopen -Dftello64=ftello -Dfseeko64=fseeko

COMMONFLAGS	:=  -g -Wall -Og -ffunction-sections \
				$(ARCH) $(DEFINES) 				 \
				-DWINDOW_NAME=\"EdiZon\"		 \
				-DVERSION_MAJOR=${VERSION_MAJOR} \
				-DVERSION_MINOR=${VERSION_MINOR} \
				-DVERSION_MICRO=${VERSION_MICRO} \
				-DVERSION_STRING=\"$(subst $(SPACE),\$(SPACE),${APP_VERSION})\" \
				$(INCLUDE) $(EXT_INCLUDE) -D__SWITCH__

CFLAGS		:=	$(COMMONFLAGS) -Wno-incompatible-pointer-types

CXXFLAGS	:=  $(COMMONFLAGS) -Wno-reorder -frtti -fexceptions -std=gnu++17

ASFLAGS		:=	-g $(ARCH)
LDFLAGS		:=	-specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS		:=  -lpython3.8m -lcurl -lmbedtls -lmbedx509 -lmbedcrypto -lnx -lz -lstdc++fs

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS		:=  $(PORTLIBS) $(LIBNX) $(BOREALIS_PATH) $(CURDIR)/libs/json $(CURDIR)/libs/libpython

export BOREALIS_PATH := libs/borealis
include $(TOPDIR)/$(BOREALIS_PATH)/library/borealis.mk

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(OUTDIR)/EdiZon
export TOPDIR	:=	$(CURDIR)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES			:=	$(filter-out $(EXCLUDES),$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c))))
CPPFILES		:=	$(filter-out $(EXCLUDES),$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp))))
SFILES			:=	$(filter-out $(EXCLUDES),$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s))))
BINFILES		:=	$(filter-out $(EXCLUDES),$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*))))

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

export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES))
export OFILES_SRC	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export OFILES 		:=	$(OFILES_BIN) $(OFILES_SRC)
export HFILES_BIN	:=	$(addsuffix .h,$(subst .,_,$(BINFILES)))

export INCLUDE		:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
						$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
						-I$(CURDIR)/$(BUILD)

export EXT_INCLUDE	:=	$(foreach dir,$(EXT_INCLUDES),-I$(dir))

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

ifeq ($(strip $(ICON)),)
	icons := $(wildcard *.jpg)
	ifneq (,$(findstring $(TARGET).jpg,$(icons)))
		export APP_ICON := $(TOPDIR)/$(TARGET).jpg
	else
		ifneq (,$(findstring icon.jpg,$(icons)))
			export APP_ICON := $(TOPDIR)/icon.jpg
		endif
	endif
else
	export APP_ICON := $(TOPDIR)/$(ICON)
endif

ifeq ($(strip $(NO_ICON)),)
	export NROFLAGS += --icon=$(APP_ICON)
endif

ifneq ($(ROMFS),)
	export NROFLAGS += --romfsdir=$(CURDIR)/$(ROMFS)
endif

ifeq ($(strip $(NO_NACP)),)
	export NROFLAGS += --nacp=$(CURDIR)/$(OUTDIR)/EdiZon.nacp
endif

.PHONY: $(BUILD) clean all install stacktrace  $(ROMFS)

#---------------------------------------------------------------------------------
all: $(BUILD) $(ROMFS)

$(BUILD):
	@[ -d $@ ] || mkdir -p $@ $(BUILD) $(OUTDIR)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo " RM   " $(BUILD) $(OUTDIR) $(ROMFS)/sysmodule
	@rm -fr $(BUILD) $(OUTDIR) $(ROMFS)/sysmodule

#---------------------------------------------------------------------------------
install: all
ifneq ($(strip $(IP)),)
	@echo " NXLNK" $(IP)
	@nxlink -s $(OUTDIR)/EdiZon.nro -a $(IP)
else
	@echo " NXLNK -Switch-"
	@nxlink -s $(OUTDIR)/EdiZon.nro
endif

#---------------------------------------------------------------------------------
stacktrace:
	@echo " PY   " get_stacktrace.py $(OUTDIR)/EdiZon.elf $(IP) 5000
	@python3 debug/get_stacktrace.py $(OUTDIR)/EdiZon.elf $(IP) 5000

#---------------------------------------------------------------------------------
else
.PHONY:	all  $(ROMFS)

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all	:	$(OUTPUT).nro $(ROMFS)

$(OUTPUT).nro	:	$(OUTPUT).elf $(OUTPUT).nacp

$(OUTPUT).elf	:	$(OFILES)

$(OFILES_SRC)	: $(HFILES_BIN)

#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------
%.bin.o	%_bin.h :	%.bin
#---------------------------------------------------------------------------------
	@echo " BIN  " $@
	@$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------