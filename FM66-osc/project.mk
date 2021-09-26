# #############################################################################
# Project Customization
# #############################################################################

include $(PLATFORMDIR)/osc.mk

PROJECT = FM66

UCSRC = 

UCXXSRC = ../src/fm64.cpp

UINCDIR = $(PROJECTDIR)/../inc

UDEFS = -DOP6 -DOPSIX -DSY77 -DBANK_COUNT=3 -DWF16 -DSHAPE_LFO -DPEG -DEGLUT11 -DEGLUTX16 -DFEEDBACK_COUNT=2

ULIB = 

ULIBDIR =
