# #############################################################################
# Project Customization
# #############################################################################

include $(PLATFORMDIR)/osc.mk

PROJECT = FM64

UCSRC = 

UCXXSRC = ../src/fm64.cpp

UINCDIR = $(PROJECTDIR)/../inc

UDEFS = -DOP6 -DOPSIX -DSY77 -DBANK_COUNT=4 -DWFSIN16 -DSHAPE_LFO -DPEG -DEGLUT11 -DEGLUTX16 -DMOD16

ULIB = 

ULIBDIR =
