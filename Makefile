include $(wildcard *.d)

#CC	= gcc
CFLAGS	= -O1 -g -Wall `pkg-config --cflags gtk+-2.0` -std=c99
LDFLAGS = `pkg-config --libs gtk+-2.0` -lX11 -Wl,-O1 -s
OBJECTS	= pimply.o callbacks.o config.o
PREFIX	= /usr/local
INSTALL	= -m 755
BINARY  = pimply

.PHONY: all clean install uninstall
all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(BINARY) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -MMD -c -o $@ $<

clean:
		rm -f $(OBJECTS) *.d $(BINARY) *~

install:
		install $(INSTALL) $(BINARY) $(PREFIX)/bin

uninstall:	
		rm $(PREFIX)/bin/$(BINARY)
