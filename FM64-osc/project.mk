# #############################################################################
# Project Customization
# #############################################################################

include $(PLATFORMDIR)/osc.mk

PROJECT = FM64

UCSRC = 

UCXXSRC = ../src/fm64.cpp

#UASMSRC = ../src/fm64.s

#UASMXSRC = ../src/fm64.S

UINCDIR = $(PROJECTDIR)/../inc

UDEFS = -DOP6 -DOPSIX -DSY77 -DBANK_COUNT=4 -DWFSIN16 -DSHAPE_LFO -DPEG -DEGLUT11 -DEGLUTX16 -DFEEDBACK_COUNT=1 -DWAVE_WIDTH

#UADEFS =

ULIB = 

ULIBDIR =
