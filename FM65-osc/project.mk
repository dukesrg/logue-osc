# #############################################################################
# Project Customization
# #############################################################################

include $(PLATFORMDIR)/osc.mk

PROJECT = FM65

UCSRC = 

UCXXSRC = ../src/fm64.cpp

UINCDIR = $(PROJECTDIR)/../inc

UDEFS = -DOP6 -DOPSIX -DSY77 -DBANK_COUNT=4 -DWFSIN16 -DSHAPE_LFO -DPEG -DEGLUT11 -DEGLUTX16 -DMOD16 -DCUSTOM_ALGORITHM_COUNT=16

ULIB = 

ULIBDIR =
