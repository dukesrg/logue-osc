# #############################################################################
# Project Customization
# #############################################################################

include $(PLATFORMDIR)/osc.mk

PROJECT = FM66

UCSRC = 

UCXXSRC = ../src/fm64.cpp

UINCDIR = $(PROJECTDIR)/../inc

UDEFS = -DOP6 -DOPSIX -DBANK_COUNT=2 -DWF16 -DFEEDBACK -DSHAPE_LFO -DCUSTOM_PARAMS -DPEG

ULIB = 

ULIBDIR =
