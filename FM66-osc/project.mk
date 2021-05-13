# #############################################################################
# Project Customization
# #############################################################################

include $(PLATFORMDIR)/osc.mk

PROJECT = FM66

UCSRC = ../fastermath/src/exp.c ../fastermath/src/log.c

UCXXSRC = ../src/fm64.cpp

UINCDIR = $(PROJECTDIR)/../inc $(PROJECTDIR)/../fastermath/include

UDEFS = -DOP6 -DOPSIX -DBANK_COUNT=3 -DWF16 -DFEEDBACK -DSHAPE_LFO -DCUSTOM_PARAMS -DFASTERMATH_POWF -D_FM_INTERNAL -D_FM_ALIGN=4

ULIB = 

ULIBDIR =
