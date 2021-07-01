# #############################################################################
# Project Customization
# #############################################################################

include $(PLATFORMDIR)/osc.mk

PROJECT = FM64EG

UCSRC = 

UCXXSRC = ../src/fm64.cpp

UINCDIR = $(PROJECTDIR)/../inc

UDEFS = -DOP6 -DOPSIX -DBANK_COUNT=2 -DWFSIN16 -DFEEDBACK -DSHAPE_LFO -DCUSTOM_PARAMS -DPEG -DEGLUT13 -DEGLUTX16

ULIB = 

ULIBDIR =
