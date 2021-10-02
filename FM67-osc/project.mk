# #############################################################################
# Project Customization
# #############################################################################

include $(PLATFORMDIR)/osc.mk

PROJECT = FM67

UCSRC = 

UCXXSRC = ../src/fm64.cpp

UINCDIR = $(PROJECTDIR)/../inc

UDEFS = -DOP6 -DOPSIX -DSY77 -DBANK_COUNT=2 -DWF16x2 -DSHAPE_LFO -DPEG -DEGLUT11 -DEGLUTX16 -DFEEDBACK_COUNT=1 -DWAVE_WIDTH

ULIB = 

ULIBDIR =
