# #############################################################################
# Project Customization
# #############################################################################

include $(PLATFORMDIR)/osc.mk

PROJECT = FM68

UCSRC = 

UCXXSRC = ../src/fm64.cpp

UINCDIR = $(PROJECTDIR)/../inc

UDEFS = -DOP6 -DOPSIX -DSY77 -DBANK_COUNT=2 -DWFSIN16 -DSHAPE_LFO -DPEG -DEGLUT13 -DEGLUTX16

ULIB = 

ULIBDIR =
