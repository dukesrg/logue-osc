BUILD := $(addsuffix -build, $(wildcard *osc))
CLEAN := $(addsuffix -clean, $(wildcard *osc))
PROJECTDIR = $(abspath .)
PLATFORMDIR = ./logue-sdk/platform
PLATFORMS = prologue minilogue-xd nutekt-digital

all: $(BUILD)

%-build: %
	@for platform in $(PLATFORMS) ; do \
	$(MAKE) -f osc.mk PLATFORMDIR=$(PLATFORMDIR)/$$platform PLATFORM=$$platform PROJECTDIR=$< ; \
	$(MAKE) -f osc.mk PLATFORMDIR=$(PLATFORMDIR)/$$platform PROJECTDIR=$< clean ; \
	done

clean: $(CLEAN)

%-clean: %
	@for platform in $(PLATFORMS) ; do \
	$(MAKE) -f osc.mk PLATFORMDIR=$(PLATFORMDIR)/$$platform PROJECTDIR=$< clean-remove ; \
	done

.PHONY: all clean
