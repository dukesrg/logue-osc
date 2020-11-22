# #############################################################################
# Project Customization
# #############################################################################

include $(PLATFORMDIR)/osc.mk

PROJECT = FM48

UCSRC = 

UCXXSRC = ../src/fm64.cpp

UINCDIR = $(PROJECTDIR)/../inc

UDEFS = -DOP4 -DOPSIX -DBANK_COUNT=4 -DWF8 -DFEEDBACK -DSHAPE_LFO

ULIB = 

ULIBDIR =
