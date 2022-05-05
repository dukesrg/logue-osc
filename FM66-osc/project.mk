# #############################################################################
# Project Customization
# #############################################################################

include $(PLATFORMDIR)/osc.mk

PROJECT = FM66

UCSRC = 

UCXXSRC = ../src/fm64.cpp

UINCDIR = $(PROJECTDIR)/../inc

UDEFS = -DOP6 -DOPSIX -DSY77 -DBANK_COUNT=4 -DWF16 -DPEG -DEGLUT11 -DEGLUTX16 -DCUSTOM_PARAMS_CYCLE

ULIB = 

ULIBDIR =
