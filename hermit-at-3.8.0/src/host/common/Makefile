#
# Copyright (c) 2008 Atmark-Techno, Inc.  All Rights Reserved.
#
include $(HERMIT_CONFIG)

ifeq ($(CONFIG_HOST_BUILD),y)

host-cflags-$(CONFIG_HOST_FUNC_FIRMUPDATE) += -DAJ_FIRMUPDATE_SUPPORT

HOST_EXTRACFLAGS += -DHERMIT_VERSION='"$(HERMIT_VERSION)"' -D_CONSOLE \
		    $(host-cflags-y)

hermit-objs := crc.o download.o erase.o eth.o ethutil.o flash.o main.o \
	       memmap.o options.o region.o serial.o target.o util.o console.o \
	       md5sum.o

hostprogs-y := hermit

always :=$(hostprogs-y)

endif

quiet_cmd_install ?= INSTALL $(always)
      cmd_install ?= mkdir -p $(2); cp $(always) $(2)
install:
	$(if $(always),$(call cmd,install,$(bindir)))
