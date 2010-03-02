###########################################################################
#
#   winui.mak
#
#   winui (MameUI) makefile
#
#   Copyright (c) 1996-2007, Nicola Salmoria and the MAME Team.
#   Visit http://mamedev.org for licensing and usage restrictions.
#
###########################################################################


###########################################################################
#################   BEGIN USER-CONFIGURABLE OPTIONS   #####################
###########################################################################


###########################################################################
##################   END USER-CONFIGURABLE OPTIONS   ######################
###########################################################################


#-------------------------------------------------
# object and source roots
#-------------------------------------------------

# add ui specific src/objs
UISRC = $(SRC)/osd/winui
UIOBJ = $(OBJ)/osd/winui

OBJDIRS += $(UIOBJ)

CFLAGS += -I $(UISRC)

#-------------------------------------------------
# configure the resource compiler
#-------------------------------------------------

RC = @windres --use-temp-file

RCDEFS = -DNDEBUG -D_WIN32_IE=0x0501

# include UISRC direcotry
RCFLAGS = -O coff --include-dir $(UISRC) --include-dir $(UIOBJ)



#-------------------------------------------------
# preprocessor definitions
#-------------------------------------------------

DEFS += \
	-DWINVER=0x0500 \
	-D_WIN32_IE=0x0501



#-------------------------------------------------
# Windows-specific flags and libraries
#-------------------------------------------------

LIBS += \
	-lkernel32 \
	-lshell32 \
	-lcomdlg32 \
	-lshlwapi \
	-lcomctl32 \
	-ladvapi32 \
	-lddraw \
	-ldinput \
	-ldxguid

ifneq ($(MSVC_BUILD),)
LIBS += -lhtmlhelp
endif



#-------------------------------------------------
# OSD Windows library
#-------------------------------------------------

# add UI objs
OSDOBJS += \
	$(UIOBJ)/mui_util.o \
	$(UIOBJ)/directinput.o \
	$(UIOBJ)/dijoystick.o \
	$(UIOBJ)/directdraw.o \
	$(UIOBJ)/directories.o \
	$(UIOBJ)/mui_audit.o \
	$(UIOBJ)/columnedit.o \
	$(UIOBJ)/screenshot.o \
	$(UIOBJ)/treeview.o \
	$(UIOBJ)/splitters.o \
	$(UIOBJ)/bitmask.o \
	$(UIOBJ)/datamap.o \
	$(UIOBJ)/dxdecode.o \
	$(UIOBJ)/picker.o \
	$(UIOBJ)/properties.o \
	$(UIOBJ)/tabview.o \
	$(UIOBJ)/help.o \
	$(UIOBJ)/history.o \
	$(UIOBJ)/dialogs.o \
	$(UIOBJ)/mui_opts.o \
	$(UIOBJ)/layout.o \
	$(UIOBJ)/datafile.o \
	$(UIOBJ)/dirwatch.o \
	$(UIOBJ)/winui.o \
	$(UIOBJ)/helpids.o \
	$(UIOBJ)/translate.o \


ifneq ($(USE_CUSTOM_UI_COLOR),)
OSDOBJS += $(UIOBJ)/paletteedit.o
endif

# add our UI resources
GUIRESFILE += $(UIOBJ)/mameui.res

$(LIBOSD): $(OSDOBJS)




#-------------------------------------------------
# rules for creating helpids.c 
#-------------------------------------------------

$(UISRC)/helpids.c : $(UIOBJ)/mkhelp$(EXE) $(UISRC)/resource.h $(UISRC)/resource.hm $(UISRC)/mameui.rc
	@"$(UIOBJ)/mkhelp$(EXE)" $(UISRC)/mameui.rc >$@

# rule to build the generator
$(UIOBJ)/mkhelp$(EXE): $(UIOBJ)/mkhelp.o $(LIBOCORE)
	@echo Linking $@...
	$(LD) $(LDFLAGS) $(OSDBGLDFLAGS) $^ $(LIBS) -o $@



#-------------------------------------------------
# rule for making the verinfo tool
#-------------------------------------------------

#VERINFO = $(UIOBJ)/verinfo$(EXE)

#$(VERINFO): $(UIOBJ)/verinfo.o $(LIBOCORE)
#	@echo Linking $@...
#	$(LD) $(LDFLAGS) $^ $(LIBS) -o $@

#BUILD += $(VERINFO)



#-------------------------------------------------
# Specific rele to compile verinfo util.
#-------------------------------------------------

#$(BUILDOBJ)/verinfo.o : $(BUILDSRC)/verinfo.c
#	@echo Compiling $<...
#	@echo $(CC) -DWINUI $(CDEFS) $(CFLAGS) -c $< -o $@
#	$(CC) -DWINUI $(CDEFS) $(CFLAGS) -c $< -o $@



#-------------------------------------------------
# generic rule for the resource compiler for UI
#-------------------------------------------------

$(GUIRESFILE): $(UISRC)/mameui.rc $(UIOBJ)/mamevers.rc
	@echo Compiling mameui resources $<...
	$(RC) $(RCDEFS) $(RCFLAGS) -o $@ -i $<



#-------------------------------------------------
# rules for resource file
#-------------------------------------------------

$(UIOBJ)/mamevers.rc: $(OBJ)/build/verinfo$(EXE) $(SRC)/version.c
	@echo Emitting $@...
	@"$(OBJ)/build/verinfo$(EXE)" -b winui $(SRC)/version.c > $@




#-------------------------------------------------
# CORE functions
# only definitions RCDEFS for mameui.rc
#-------------------------------------------------

ifneq ($(USE_CUSTOM_UI_COLOR),)
RCDEFS += -DUI_COLOR_PALETTE
endif

ifneq ($(USE_AUTO_PAUSE_PLAYBACK),)
RCDEFS += -DAUTO_PAUSE_PLAYBACK
endif

ifneq ($(USE_SCALE_EFFECTS),)
RCDEFS += -DUSE_SCALE_EFFECTS
endif

ifneq ($(USE_TRANS_UI),)
RCDEFS += -DTRANS_UI
endif

ifneq ($(USE_JOYSTICK_ID),)
RCDEFS += -DJOYSTICK_ID
endif

ifneq ($(USE_VOLUME_AUTO_ADJUST),)
RCDEFS += -DUSE_VOLUME_AUTO_ADJUST
endif

ifneq ($(USE_DRIVER_SWITCH),)
RCDEFS += -DDRIVER_SWITCH
endif



#-------------------------------------------------
# MAMEUI-specific functions
#-------------------------------------------------

ifneq ($(USE_MISC_FOLDER),)
DEFS += -DMISC_FOLDER
RCDEFS += -DMISC_FOLDER
endif

ifneq ($(USE_SHOW_SPLASH_SCREEN),)
DEFS += -DUSE_SHOW_SPLASH_SCREEN
RCDEFS += -DUSE_SHOW_SPLASH_SCREEN
endif

ifneq ($(USE_STORY_DATAFILE),)
DEFS += -DSTORY_DATAFILE
RCDEFS += -DSTORY_DATAFILE
endif

ifneq ($(USE_VIEW_PCBINFO),)
DEFS += -DUSE_VIEW_PCBINFO
RCDEFS += -DUSE_VIEW_PCBINFO
endif

ifneq ($(USE_TREE_SHEET),)
DEFS += -DTREE_SHEET
RCDEFS += -DTREE_SHEET
endif

ifneq ($(USE_SHOW_UNAVAILABLE_FOLDER),)
DEFS += -DSHOW_UNAVAILABLE_FOLDER
endif




#####  End windui.mak ##############################################

