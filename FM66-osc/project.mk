# #############################################################################
# Project Customization
# #############################################################################

include $(PLATFORMDIR)/osc.mk

PROJECT = FM66

UCSRC = 

UCXXSRC = ../src/fm64.cpp

UINCDIR = $(PROJECTDIR)/../inc

UDEFS = -DOP6 -DOPSIX -DBANK_COUNT=3 -DWF16 -DFEEDBACK -DSHAPE_LFO -DCUSTOM_PARAMS

ULIB = 

ULIBDIR =
