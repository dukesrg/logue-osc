# #############################################################################
# Project Customization
# #############################################################################

include $(PLATFORMDIR)/osc.mk

PROJECT = FM69

UCSRC = 

UCXXSRC = ../src/fm64.cpp

UINCDIR = $(PROJECTDIR)/../inc

UDEFS = -DOP6 -DOPSIX -DBANK_COUNT=4 -DWFSIN16 -DFEEDBACK -DSHAPE_LFO -DCUSTOM_PARAMS -DKIT_MODE

ULIB = 

ULIBDIR =