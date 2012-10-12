# disable bullshit ass implicit rules; i'll be doing shit explicitly
export MAKEFLAGS += --no-builtin-rules
.SUFFIXES:

CFLAGS	= -std=gnu99 -O0 -g -Wall -pipe `pkg-config --cflags gtk+-2.0` 
LDFLAGS = `pkg-config --libs gtk+-2.0` -lX11 -Wl,-O1
OBJECTS := $(patsubst %.c, %.o, $(wildcard *.c))
PREFIX	= /usr/local
INSTALL	= -m 755
BINARY  = pimply

.PHONY: all clean install uninstall
all: $(OBJECTS)
	$(CC) $(OBJECTS) -o $(BINARY) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJECTS) *.d $(BINARY) *~

install:
	install $(INSTALL) $(BINARY) $(PREFIX)/bin

uninstall:	
	rm $(PREFIX)/bin/$(BINARY)
