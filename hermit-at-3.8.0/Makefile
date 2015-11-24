#
# Copyright (c) 2006 Atmark Techno, Inc.  All Rights Reserved.
#
# The default target of this Makefile is...

srctree = $(shell pwd)
HERMIT_CONFIG = $(srctree)/.config
export srctree HERMIT_CONFIG

INSTALLPREFIX := $(HOME)
bindir = $(INSTALLPREFIX)/bin
libdir = $(INSTALLPREFIX)/lib/hermit
export bindir libdir

dot-config := $(shell test ! -e $(HERMIT_CONFIG) || echo 1)

HOSTCC		= gcc
HOSTCXX		= g++
HOSTCFLAGS	= -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer
HOSTCXXFLAGS	= -O2

ifdef V
  ifeq ("$(origin V)", "command line")
    KBUILD_VERBOSE = $(V)
  endif
endif
ifndef KBUILD_VERBOSE
  KBUILD_VERBOSE = 0
endif

ifeq ($(KBUILD_VERBOSE),1)
  quiet =
  Q =
else
  quiet = quiet_
  Q = @
  MAKE = make -s
endif

export HOSTCC HOSTCXX HOSTCFLAGS HOSTCXXFLAGS
export quiet Q KBUILD_VERBOSE

all::
	$(Q)test -e $(HERMIT_CONFIG) || (                                  \
		echo "***";                                                \
		echo "*** You have not yet configured your hermit!";       \
		echo "*** (missing hermit .config file)";                  \
		echo "***" ;                                               \
		echo "*** Please run some configurator ";                  \
		echo "*** (e.g. \"make config\" or \"make menuconfig\")."; \
		echo "***" ;                                               \
		/bin/false)

clean distclean scrub::
# This program has not yet been configured,
# so nothing needs to be done here.
	@/bin/true

include scripts/Kbuild.include

config:
	$(Q)$(MAKE) -s $(build)=scripts/basic
	$(Q)$(MAKE) -s $(build)=scripts/kconfig $@
	$(Q)test -e $(HERMIT_CONFIG) && \
	scripts/config2header.sh $(HERMIT_CONFIG) include/autoconf.h
	@ln -fs ../autoconf.h include/target/autoconf.h
	@ln -fs ../version.h include/target/version.h
%config:
	$(Q)$(MAKE) -s $(build)=scripts/basic
	$(Q)$(MAKE) -s $(build)=scripts/kconfig $@
	$(Q)test -e $(HERMIT_CONFIG) && \
	scripts/config2header.sh $(HERMIT_CONFIG) include/autoconf.h
	@ln -fs ../autoconf.h include/target/autoconf.h
	@ln -fs ../version.h include/target/version.h

clean = -f scripts/Makefile.clean obj
config_clean:
	$(Q)$(MAKE) -s $(clean)=scripts
	find scripts \
		-name '*.[ao]' -o -name '.*.cmd' -o -name '.*.d' \
		| xargs rm -f
	rm -f .config .config.old .config.cmd .tmpconfig.h
	rm -f include/autoconf.h include/target/autoconf.h
	rm -f include/target/version.h

ifeq ($(dot-config),1)
include $(HERMIT_CONFIG)

#
# Fail to build unless 'make scrub' is run between targets.  This
# minimizes the chance that stale object files will be incorporated
# into a target build.
#
ifneq (,$(wildcard target-stamp))
	last-target := $(shell cat target-stamp)
	ifneq ($(CONFIG_PLATFORM_NAME),"$(last-target)")
		error := target-error $(CONFIG_PLATFORM_NAME)
	endif
endif

HERMIT-VERSION-FILE: .FORCE-HERMIT-VERSION-FILE
	$(Q)$(SHELL_PATH) ./GIT-VERSION-GEN
-include HERMIT-VERSION-FILE
export HERMIT_VERSION

include/version.h: HERMIT-VERSION-FILE .FORCE-include/version.h
	@echo '#ifndef _HERMIT_VERSION_H_' > $@
	@echo '#define _HERMIT_VERSION_H_' >> $@
	@echo '#define HERMIT_VERSION "$(HERMIT_VERSION)"' >> $@
	@echo '#define BUILD_TIME "$(shell LANG=C date +"%T, %b %d %Y")"' >> $@
	@echo '#endif' >> $@

.PHONY: $(error) all build-host build-target .FORCE-HERMIT-VERSION-FILE .FORCE-include/version.h
all:: $(error) build
########################################################################

target-error:
	@echo "previous TARGET:" `cat target-stamp`
	@echo "target-stamp mismatch; 'make scrub' to switch targets"
	@/bin/false
target-stamp:
ifeq ($(CONFIG_TARGET_BUILD),y)
	@ln -s ../arch-$(CONFIG_PLATFORM_ARCH_NAME) include/target/arch
	@ln -s $(CONFIG_PLATFORM_NAME) src/target/platform
	@echo $(CONFIG_PLATFORM_NAME) > $@
endif

.PHONY: build build-version build-host build-target
build:: build-version build-host build-target
build-version: include/version.h
build-host: build-version
ifeq ($(CONFIG_HOST_BUILD),y)
	$(Q)$(MAKE) $(build)=src/host/common
endif
build-target: build-version target-stamp
ifeq ($(CONFIG_TARGET_BUILD),y)
	$(Q)$(MAKE) $(build)=src/target
endif

.PHONY: clean clean-host clean-target
clean:: clean-host clean-target
	rm -f HERMIT-VERSION-FILE
	rm -f include/version.h
clean-host:
	$(Q)$(MAKE) -s $(clean)=src/host/common
	find src/host/common \
		-name '*.[ao]' -o -name '.*.cmd' -o -name '.*.d' \
		| xargs rm -f
clean-target:
	$(Q)$(MAKE) -s $(clean)=src/target
	find src/target \
		-name '*.[ao]' -o -name '.*.cmd' -o -name '.*.d' \
		-o -name '*.elf' -o -name '*.bin' -o -name '*.map' \
		-o -name '*.srec' \
		-o -name 'loader.lds' -o -name 'profile-stamp' \
		| xargs rm -f

.PHONY: scrub scrub-host scrub-target
scrub:: clean scrub-host scrub-target config_clean
	rm -f target-stamp
	rm -f src/target/platform include/target/arch
scrub-host: clean-host
scrub-target: clean-target

.PHOHY: distclean
distclean:: scrub
	find . -name '*~' \
		| xargs rm -f

.PHONY: install install-host install-target
install: install-host install-target
install-host: build-host
	$(Q)$(MAKE) $(build)=src/host/common install
install-target: build-target
	$(Q)$(MAKE) $(build)=src/target install

endif
