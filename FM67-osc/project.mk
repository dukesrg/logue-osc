# #############################################################################
# Project Customization
# #############################################################################

include $(PLATFORMDIR)/osc.mk

PROJECT = FM67

UCSRC = 

UCXXSRC = ../src/fm64.cpp

UINCDIR = $(PROJECTDIR)/../inc

UDEFS = -DOP4 -DOP6 -DOPSIX -DBANK_COUNT=3 -DWF16x2 -DFEEDBACK -DSHAPE_LFO -DCUSTOM_PARAMS -DPEG

ULIB = 

ULIBDIR =
