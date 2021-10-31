# #############################################################################
# Project Customization
# #############################################################################

include $(PLATFORMDIR)/osc.mk

PROJECT = Morpheus

UCSRC =

UCXXSRC = ../src/morpheus.cpp

UINCDIR = $(PROJECTDIR)/../inc

UDEFS = -DFORMAT_PCM12 -DSAMPLE_GUARD

ULIBS =

ULIBDIR =
