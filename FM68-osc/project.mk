# #############################################################################
# Project Customization
# #############################################################################

include $(PLATFORMDIR)/osc.mk

PROJECT = FM68

UCSRC = 

UCXXSRC = ../src/fm64.cpp

UINCDIR = $(PROJECTDIR)/../inc

UDEFS = -DOP6 -DOPSIX -DBANK_SELECT -DBANK_COUNT=2 -DWF32 -DFEEDBACK -DSHAPE_LFO

ULIB = 

ULIBDIR =
