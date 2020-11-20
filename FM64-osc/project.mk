# #############################################################################
# Project Customization
# #############################################################################

include $(PLATFORMDIR)/osc.mk

PROJECT = FM64

UCSRC = 

UCXXSRC = ../src/fm64.cpp

UINCDIR = $(PROJECTDIR)/../inc

#UDEFS = -DOP4 -DOP6 -DOPSIX -DBANK_COUNT=4 -DFEEDBACK -DSHAPE_LFO
UDEFS = -DOP6 -DOPSIX -DBANK_COUNT=4 -DFEEDBACK -DSHAPE_LFO

ULIB = 

ULIBDIR =
