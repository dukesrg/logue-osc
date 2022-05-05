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

UDEFS = -DOP6 -DOPSIX -DSY77 -DBANK_COUNT=5 -DWFSIN16 -DPEG -DEGLUT11 -DEGLUTX16 -DCUSTOM_PARAMS_CYCLE

#UADEFS =

ULIB = 

ULIBDIR =
