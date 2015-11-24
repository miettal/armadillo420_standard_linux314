#
# Copyright (c) 2008 Atmark-Techno, Inc.  All Rights Reserved.
#

obj-y += assert.o
obj-y += command.o
obj-y += crc.o
obj-y += entry.o
obj-y += gunzip.o
obj-y += loader.o
obj-$(CONFIG_HAS_MMU) += boost.o
obj-$(CONFIG_HAS_CACHE) += boost.o
obj-$(CONFIG_ARCH_ARM) += boost_arm.o
obj-y += print.o
obj-y += scan.o
obj-y += map.o
obj-$(CONFIG_ARCH_MICROBLAZE) += microblaze.o
obj-$(CONFIG_ARCH_PPC) += powerpc.o

#
# Library
#
lib-y += ctype.o
lib-y += delay.o
lib-y += eth_util.o
lib-y += led.o
lib-$(CONFIG_ARCH_ARM) += lib1funcs.o
lib-y += mac.o

lib-y += memcmp.o
lib-y += memcpy.o
lib-y += memset.o
lib-y += memzero.o
lib-y += strchr.o
lib-y += strcmp.o
lib-y += strcpy.o
lib-y += strlen.o
lib-y += strncmp.o
lib-y += strncpy.o
lib-y += strpbrk.o
lib-y += strrchr.o
lib-y += strstr.o
lib-y += strtol.o

lib-y += ffs.o
lib-y += fls.o

lib-y += list.o
lib-y += mm.o
