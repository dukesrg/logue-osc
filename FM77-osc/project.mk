# #############################################################################
# Project Customization
# #############################################################################

include $(PLATFORMDIR)/osc.mk

PROJECT = FM77

UCSRC = 

UCXXSRC = ../src/fm64.cpp

UINCDIR = $(PROJECTDIR)/../inc

#UDEFS = -DOP6 -DOPSIX -DBANK_COUNT=4 -DWFSIN16 -DFEEDBACK -DCUSTOM_PARAMS -DKIT_MODE -DPEG
UDEFS = -DOP6 -DOPSIX -DBANK_COUNT=4 -DWFSIN16 -DCUSTOM_PARAMS -DKIT_MODE -DPEG

ULIB = 

ULIBDIR =
