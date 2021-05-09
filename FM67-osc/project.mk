# #############################################################################
# Project Customization
# #############################################################################

include $(PLATFORMDIR)/osc.mk

PROJECT = FM67

UCSRC = 

UCXXSRC = ../src/fm64.cpp

UINCDIR = $(PROJECTDIR)/../inc

UDEFS = -DOP6 -DOPSIX -DBANK_SELECT -DBANK_COUNT=2 -DWF16x2 -DFEEDBACK -DSHAPE_LFO -DPEG

ULIB = 

ULIBDIR =
