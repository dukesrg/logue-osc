# #############################################################################
# Project Customization
# #############################################################################

include $(PLATFORMDIR)/osc.mk

PROJECT = Morpheus-U

UCSRC =

UCXXSRC = ../src/morpheus.cpp

UINCDIR = $(PROJECTDIR)/../inc

UDEFS = -DFORMAT_ULAW

ULIBS =

ULIBDIR =
