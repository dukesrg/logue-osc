# #############################################################################
# Project Customization
# #############################################################################

include $(PLATFORMDIR)/osc.mk

PROJECT = FM48

UCSRC = 

UCXXSRC = ../src/fm64.cpp

UINCDIR = $(PROJECTDIR)/../inc

UDEFS = -DOP4 -DOPSIX -DSY77 -DBANK_COUNT=4 -DWF8 -DSHAPE_LFO -DPEG -DEGLUT11 -DEGLUTX16

ULIB = 

ULIBDIR =
