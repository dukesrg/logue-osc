BUILD := $(addsuffix -build, $(wildcard *osc))
CLEAN := $(addsuffix -clean, $(wildcard *osc))
PROJECTDIR = $(abspath .)
PLATFORMDIR = ./logue-sdk/platform/nutekt-digital

all: $(BUILD)

%-build: %
	@$(MAKE) -f osc.mk PLATFORMDIR=$(PLATFORMDIR) PROJECTDIR=$<

clean: $(CLEAN)

%-clean: %
	@$(MAKE) -f osc.mk PLATFORMDIR=$(PLATFORMDIR) PROJECTDIR=$< clean-remove

.PHONY: all clean
