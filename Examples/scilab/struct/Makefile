TOP        = ../..
SWIGEXE    = $(TOP)/../swig
SWIG_LIB_DIR = $(TOP)/../$(TOP_BUILDDIR_TO_TOP_SRCDIR)Lib
SRCS       =
TARGET     = example
INTERFACE  = example.i

check: build
	$(MAKE) -f $(TOP)/Makefile SRCDIR='$(SRCDIR)' scilab_run

build:
	$(MAKE) -f $(TOP)/Makefile SRCS='$(SRCS)' SRCDIR='$(SRCDIR)' \
	SWIG_LIB_DIR='$(SWIG_LIB_DIR)' SWIGEXE='$(SWIGEXE)' \
	TARGET='$(TARGET)' INTERFACE='$(INTERFACE)' scilab

clean:
	$(MAKE) -f $(TOP)/Makefile SRCDIR='$(SRCDIR)' scilab_clean
