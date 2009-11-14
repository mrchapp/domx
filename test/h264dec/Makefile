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

CFLAGS         = 
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

TARGETNAME  = h264dectest


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
src/H264_Decoder_ILClient.c \
src/H264_Decoder_ILClient_TestCases.c




# Search path for include files

INCLUDES    = \
    $(BRIDGEROOT)/syslink/api/include \
    $(PROJROOT)/../src/system/omx_core \
    $(PROJROOT)/../src/system/mm_osal \
    $(PROJROOT)/../src/mmplatform \
    ./inc \
    $(TILER_INC_PATH) \

# Libraries needed for linking.

ST_LIBS        = omx_core omx_proxy_component mmplatform domx mm_osal
SH_LIBS        = pthread rt utils procmgr ipc rcm notify
SH_LIBS        += sysmgr sysmemmgr memmgr d2cmap


# Search path for library (and linker command) files.
# Current folder and target folder are included by default.

LIBINCLUDES = $(BRIDGEROOT)/target/lib 


# Do not change below "include" line(s)

include $(PROJROOT)/make/build.mk
