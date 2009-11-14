#
#  omx_proxy_component/test
#
#  Copyright (C) 2009 Texas Instruments, Inc.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU Lesser General Public License as published
#  by the Free Software Foundation version 2.1 of the License.
#
#  This program is distributed .as is. WITHOUT ANY WARRANTY of any kind,
#  whether express or implied; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#

#  ----------------------------------------------------------------------------
#  Revision History
#
#
#      REF=ORG
#      Original version.
#  ----------------------------------------------------------------------------



include $(PROJROOT)/make/start.mk

# Do not change above "include" line(s)

# Arguments to tools, will move to make system once finalized.

CFLAGS         = -DREADLINE=1 -DOMTB_OMX_API_TIMING_ENABLE=0 -DOMTB_LINUX=0  -DOMTB_FF_VD_H264=1 -DOMTB_FF_VE_H264=1 -UNETRA -DWTSD=1 -DTILER=1 -DWTSD_TEMP=1
# -DSHARED_MEM
# -DUSE_LIBEDIT=1 -DOMTB_OMX_API_TIMING_ENABLE=0 -DOMTB_LINUX=0
CDEFS          = 
ifeq ($(BUILD),udeb)
CDEFS          += DEBUG 
endif
CDEFS          +=

EXEC_ARGS      = 
ST_LIB_ARGS    = 
SH_LIB_ARGS    = 

# Define this macro if target runs in kernel mode
#__KERNEL__ = 1

# Target name and extension
# static library        (ST_LIB): filename.a
# shared library soname (SH_LIB): filename.so.maj_ver.min_ver
# executable            (EXEC)  : filename.out

TARGETNAME  = omtb


# TARGETTYPE must be EXEC, ST_LIB or SH_LIB in upper case.

TARGETTYPE  = EXEC

# install directory relative to the HOSTTARGET directory
HOSTRELEASE = binaries

# install directory relative to the root filesystem
ROOTFSRELEASE = binaries

# Folders in which gmake will run before building current target

SUBMODULES  = \

# Filename must not begin with '.', '/' or '\'

SOURCES     = \
src/cmdLine.c \
src/dbgCmd.c \
src/mvxDbg.c \
src/omtbApi.c \
src/omtbCmd.c \
src/omtbCmdEntries.c \
src/omtbCommon.c \
src/omtbH264Vdec.c \
src/omtbH264VdecCfgCmdEntries.c \
src/omtbH264Venc.c \
src/omtbH264VencCfgCmdEntries.c \
src/omtbMain.c \
src/omtbOmxApiCmdEntries.c \
#src/omtbAudio.c \
#src/omtbDisplay.c \
#src/omtbG711Adec.c \
#src/omtbG711AdecCfgCmdEntries.c \
#src/omtbHDVPSSVfcc.c \
#src/omtbHDVPSSVfccCfgCmdEntries.c \
#src/omtbHDVPSSVfdc.c \
#src/omtbHDVPSSVfdcCfgCmdEntries.c \
#src/H264_ILClient.c \
#src/H264_ILClient_AdvancedSettings.c \
#src/H264_ILClient_BasicSettings.c \
#src/H264_ILClient_DynamicSettings.c \





# Search path for include files

INCLUDES    = \
    $(PROJROOT)/omtb/inc \
	$(BRIDGEROOT)/syslink/api/include \
    $(PROJROOT)/../src/system/omx_core \
    $(PROJROOT)/../src/system/mm_osal \
    $(PROJROOT)/../src/mmplatform \
    ./inc \
    $(TILER_INC_PATH) \

# Libraries needed for linking.

ST_LIBS        = omx_core omx_proxy_component mmplatform domx mm_osal
# For HISTORY of commands
#ST_LIBS		   += edit
SH_LIBS        = pthread rt utils procmgr ipc rcm notify
SH_LIBS        += sysmgr sysmemmgr memmgr d2cmap


# Search path for library (and linker command) files.
# Current folder and target folder are included by default.

LIBINCLUDES = $(BRIDGEROOT)/target/lib 


# Do not change below "include" line(s)

include $(PROJROOT)/make/build.mk
