# #############################################################################
# Project Customization
# #############################################################################

include $(PLATFORMDIR)/osc.mk

PROJECT = FM64

UCSRC = 

UCXXSRC = ../src/fm64.cpp

UINCDIR = $(PROJECTDIR)/../inc

UDEFS = -DOP6 -DOPSIX -DBANK_SELECT -DBANK_COUNT=3 -DWFSIN16 -DFEEDBACK -DSHAPE_LFO -DCUSTOM_PARAMS -DPEG

ULIB = 

ULIBDIR =
