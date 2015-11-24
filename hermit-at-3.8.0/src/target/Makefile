#
# src/target/Makefile
#
# Copyright (c) 2009 Atmark Techno, Inc.  All Rights Reserved.
#

include $(HERMIT_CONFIG)

comma	:= ,
empty	:=
space	:= $(empty) $(empty)

as-option = $(shell if $(CC) $(CFLAGS) $(1) -c -x assembler /dev/null \
	     -o /dev/null > /dev/null 2>&1; then echo "$(1)"; \
	     else echo "$(2)"; fi ;)
cc-option = $(shell if $(CC) $(CFLAGS) $(1) -S -o /dev/null -xc /dev/null \
	     > /dev/null 2>&1; then echo "$(1)"; else echo "$(2)"; fi ;)

CROSS_COMPILE := $(if $(CONFIG_ARCH_ARM),arm-linux-gnueabi-,$(CROSS_COMPILE))
CROSS_COMPILE := $(if $(CONFIG_CROSSDEV_ARMHF), \
			arm-linux-gnueabihf-,$(CROSS_COMPILE))
CROSS_COMPILE := $(if $(CONFIG_ARCH_MICROBLAZE),mb-,$(CROSS_COMPILE))
CROSS_COMPILE := $(if $(CONFIG_ARCH_PPC),powerpc-linux-,$(CROSS_COMPILE))

CC	:= $(CROSS_COMPILE)gcc
CPP	:= $(CC) -E
LD	:= $(CROSS_COMPILE)ld
AR	:= $(CROSS_COMPILE)ar
RANLIB	:= $(CROSS_COMPILE)ranlib
OBJCOPY	:= $(CROSS_COMPILE)objcopy

march-$(CONFIG_ARCH_ARM_32v4T)	:= -D__ARM_ARCH__=4 -march=armv4t
march-$(CONFIG_ARCH_ARM_32v5)	:= -D__ARM_ARCH__=5 -march=armv5te
march-$(CONFIG_ARCH_ARM_32v6)	:= -D__ARM_ARCH__=6 \
				   $(call cc-option, -march=armv6, \
						     -march=armv5t)
march-$(CONFIG_ARCH_ARM_32v7)	:= -D__ARM_ARCH__=7 \
				   $(call cc-option, -march=armv7-a, \
						     -march=armv5t)

mtune-$(CONFIG_ARCH_ARM_920T)	:= -mtune=arm9tdmi
mtune-$(CONFIG_ARCH_ARM_926T)	:= -mtune=arm9tdmi
mtune-$(CONFIG_ARCH_ARM_V6)	:= $(firstword \
					$(call cc-option, -mtune=arm1136jf-s) \
					$(call cc-option, -mtune=arm1136jfs) \
					$(call cc-option, -mtune=strongarm) \
					)

mflags-$(CONFIG_ARCH_ARM)	:= -mthumb-interwork -mthumb
mflags-$(CONFIG_ARCH_MICROBLAZE):= -mno-xl-soft-mul -mno-xl-soft-div
mflags-$(CONFIG_ARCH_PPC)	:= -m32 -mmultiple -mstring \
				   -Wa,-m405 -Wa,-mregnames,-m405

thumb2-$(CONFIG_ARCH_ARM_V7)	:= $(call as-option, \
					-Wa$(comma)-mimplicit-it=always, \
					-Wa$(comma)-mauto-it)

AFLAGS	:= -Wall $(thumb2-y) $(march-y) $(mtune-y) $(mflags-y) -I$(srctree)/include/target -I$(srctree)/src/target/
CFLAGS	:= -Wall -O2 -fomit-frame-pointer -static \
	   -nostdinc -nostdlib -fno-builtin \
	   $(march-y) $(mtune-y) $(mflags-y) \
	   -I$(srctree)/include/target/ \
	   -I$(srctree)/src/target/
GCC_VER := $(shell $(CC) -dumpversion | sed "s/\..*//")

KBUILD_BUILTIN := 1
KBUILD_CHECKSRC := 0
.EXPORT_ALL_VARIABLES:

build_target_prefix := $(shell echo $(CONFIG_PLATFORM_NAME)/$(shell \
			$(srctree)/tools/ncvt -c "-" \
			loader $(if $(CONFIG_MACH_NAME),\
			$(CONFIG_MACH_NAME),$(CONFIG_PLATFORM_NAME)) \
			$(CONFIG_PROFILE_NAME) v$(HERMIT_VERSION)))
build_target := $(shell echo $(obj)/$(build_target_prefix))
platform_dir := $(shell echo $(obj)/$(CONFIG_PLATFORM_NAME))

obj-y += $(CONFIG_PLATFORM_NAME)/
obj-y += core/ driver/ command/
libs-y += src/target/core/
libs-y := $(patsubst %/, %/lib.a, $(libs-y))

always := $(build_target_prefix).bin

LDSCRIPT := $(CONFIG_SYSTEM_LDSCRIPT)
quiet_cmd_build_lds ?= LDS     $@
      cmd_build_lds ?= $(CPP) -P -x assembler-with-cpp $(CFLAGS) \
		       $(obj)/platform/loader.lds.$(LDSCRIPT) > $@
$(platform_dir)/loader.lds: $(wildcard $(obj)/platform/loader.lds.*) FORCE
	$(call cmd,build_lds,)

quiet_cmd_build_elf ?= LD      $@
      cmd_build_elf ?= $(LD) \
			 -Map $(build_target).map \
			 -T$(platform_dir)/loader.lds -o $@ \
			 $(obj-y) $(libs-y) \
			 --start-group $(LIBS) --end-group
$(build_target).elf: $(platform_dir)/loader.lds FORCE
	$(call cmd,build_elf,)

OBJSECTIONS := .boot .entry .text .data .bss
OBJCOPYFLAGS := -O binary -S $(addprefix -j ,$(OBJSECTIONS))
quiet_cmd_build_bin ?= OBJCOPY $@
      cmd_build_bin ?= $(OBJCOPY) $(OBJCOPYFLAGS) $< $@
$(build_target).bin: $(build_target).elf
	$(call cmd,build_bin,)

-include $(obj)/platform/Makefile.include

install-files := $(addsuffix _install,$(addprefix $(obj)/,$(always)))
quiet_cmd_install ?= INSTALL $(subst _install,,$@)
      cmd_install ?= mkdir -p $(2); cp $(subst _install,,$@) $(2)
install: $(install-files)
%_install:
	$(call cmd,install,$(libdir))
