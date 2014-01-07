CC := gcc
CFLAGS = -I/usr/include/libusb-1.0

TARGETS = radio

INSTALL_PROGRAM = install -m 755 -p

OBJECTS_radio = \
	bitstream.o \
	fmtx.o \
	hid-libusb.o \
	logging.o \
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

radio: $(OBJECTS_radio)
	$(CC) $(CFLAGS_$@) $(OBJECTS_$@) $(LIBS_$@) $(LDFLAGS_$@) -o $@

%.o: %.c %.h Makefile
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJECTS_radio)
