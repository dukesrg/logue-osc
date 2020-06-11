# #############################################################################
# Project Customization
# #############################################################################

include $(PLATFORMDIR)/osc.mk

PROJECT = Anthologue

UCSRC = 

UCXXSRC = ../src/anthologue.cpp

UINCDIR = $(PROJECTDIR)/../inc

UDEFS =
#a trick to keep default makefile and syms
ULIBS = -Xlinker --defsym=_fx_get_bpmf=0x0807ca8c

ULIBDIR =
