#---------------------------------------------------------------------
#  $Header:$
#  Copyright (c) 2000-2007 Vivotek Inc. All rights reserved.
#
#  +-----------------------------------------------------------------+
#  | THIS SOFTWARE IS FURNISHED UNDER A LICENSE AND MAY ONLY BE USED |
#  | AND COPIED IN ACCORDANCE WITH THE TERMS AND CONDITIONS OF SUCH  |
#  | A LICENSE AND WITH THE INCLUSION OF THE THIS COPY RIGHT NOTICE. |
#  | THIS SOFTWARE OR ANY OTHER COPIES OF THIS SOFTWARE MAY NOT BE   |
#  | PROVIDED OR OTHERWISE MADE AVAILABLE TO ANY OTHER PERSON. THE   |
#  | OWNERSHIP AND TITLE OF THIS SOFTWARE IS NOT TRANSFERRED.        |
#  |                                                                 |
#  | THE INFORMATION IN THIS SOFTWARE IS SUBJECT TO CHANGE WITHOUT   |
#  | ANY PRIOR NOTICE AND SHOULD NOT BE CONSTRUED AS A COMMITMENT BY |
#  | VIVOTEK INC.                                                    |
#  +-----------------------------------------------------------------+
#
#  Project name         : NONE
#  Module name          : config.mk
#  Module description   : configuration for makefile inclusion
#  Author               : Joe Wu
#  Created at           : 2006/08/02
#  $History:$
#
#---------------------------------------------------------------------

#---------------------------------------------------------------------
# Configurations
#---------------------------------------------------------------------

# general configuration, for each configuration
#---------------------------------------------------------------------
PLATFORM 	= _LINUX
LINUXKERNEL_INSTALL_DIR = ${PRODUCTDIR}/tmp
INCDIR		+= $(LINUXKERNEL_INSTALL_DIR)/include
SYSDEFS  	= 
CFLAGS	 	= -Wall
USRDEFS  	= 
LINKFLAGS	+= -lpthread -lm 
BUILD_CONF	= armlinux_release

ifeq "$(soc_platform)" ""
LIBS           = common peripheral/iicctrl
else
LIBS		= $(common_lib) $(iicctrl_lib) 
endif

ifeq "$(soc_platform)" "dm365"
USRDEFS += _DM365
endif

ifeq "$(soc_platform)" "mozart3"
USRDEFS += _MOZART3
endif

# specific configuration
#---------------------------------------------------------------------
ifeq "$(BUILD_CONF)" "linux_release"
CFLAGS	 	= -O3 -D_NDEBUG -Wall
endif

#---------------------------------------------------------------------
ifeq "$(BUILD_CONF)" "linux_debug"
CFLAGS		= -O0 -Wall
endif

#---------------------------------------------------------------------
ifeq "$(BUILD_CONF)" "armlinux_release"
CFLAGS	 	= -O3 -D_NDEBUG -Wall
TOOL_CONFIG = ARMLinux
#USRDEFS		+= IMX035
endif

#---------------------------------------------------------------------
ifeq "$(BUILD_CONF)" "vcwin32_release"
PLATFORM 	= _WIN32_
TOOL_CONFIG = VCWin32
LIBS 		= common
CFLAGS	 	= /O2 -D_NDEBUG
endif

#---------------------------------------------------------------------
# tool settings
#---------------------------------------------------------------------

include $(MAKEINC)/tools.mk
ifeq "$(TOOL_CONFIG)" "CONFIG_NAME"
# you can add your additional tools configuration here
endif
