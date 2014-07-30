CC ?= gcc
CXX ?= g++
CFLAGS = -I/usr/include/libusb-1.0
CXXFLAGS = -std=c++11

TARGETS = radio

INSTALL_PROGRAM = install -m 755 -p

OBJECTS_radio = \
	bitstream.o \
	fmtx.o \
	hid-libusb.o \
	logging.o \
	commands.o \
	radio.o \
	$(NULL)
LIBS_radio = \
	-lusb-1.0 \
	-lm \
	$(NULL)


ARCH := $(shell uname -m)

ifeq ($(ARCH), armv6l)
# do something RaspPi-y

else
ifeq ($(filter-out i686 x86_64 armv5tel, $(ARCH)),)
# do something Linux-y

else
	VAR := $(error Unsupported architecture type)
endif
endif


.PHONY: all
all: $(TARGETS)

radio: $(OBJECTS_radio) Makefile
	$(CXX) $(CFLAGS_$@) $(OBJECTS_$@) $(LIBS_$@) $(LDFLAGS_$@) -o $@

HEADERS_radio=$(wildcard $(subst .o,.h,$(OBJECTS_radio)))

radio.o: propertydefs.h commanddefs.h logging.h fmtx.h commands.h
$(OBJECTS_radio): Makefile $(HEADERS_radio)

clean:
	rm -f $(OBJECTS_radio)
