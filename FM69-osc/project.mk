# #############################################################################
# Project Customization
# #############################################################################

include $(PLATFORMDIR)/osc.mk

PROJECT = FM69

UCSRC = 

UCXXSRC = ../src/fm64.cpp

UINCDIR = $(PROJECTDIR)/../inc

UDEFS = -DOP6 -DOPSIX -DSY77 -DBANK_COUNT=5 -DWFSIN16 -DSHAPE_LFO -DPEG -DKIT_MODE -DEGLUT11 -DEGLUTX16

ULIB = 

ULIBDIR =
