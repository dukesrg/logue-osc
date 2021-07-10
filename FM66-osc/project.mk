# #############################################################################
# Project Customization
# #############################################################################

include $(PLATFORMDIR)/osc.mk

PROJECT = FM66

UCSRC = 

UCXXSRC = ../src/fm64.cpp

UINCDIR = $(PROJECTDIR)/../inc

UDEFS = -DOP6 -DOPSIX -DBANK_COUNT=4 -DWF16 -DFEEDBACK -DSHAPE_LFO -DCUSTOM_PARAMS -DPEG -DEGLUT11 -DEGLUTX16

ULIB = 

ULIBDIR =
