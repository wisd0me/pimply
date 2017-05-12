# disable bullshit ass implicit rules; i'll be doing shit explicitly
export MAKEFLAGS += --no-builtin-rules
.SUFFIXES:

CFLAGS := -std=gnu99 -O1 -g -Wall -pipe `pkg-config --cflags gtk+-2.0`
LDFLAGS := -Wl,-O1
LDFLAGS += `pkg-config --libs x11 gtk+-2.0`
OBJDIR := objdir
OBJECTS := $(patsubst %.c, %.o, $(wildcard *.c))
OBJECTS := $(addprefix $(OBJDIR)/, $(OBJECTS))
PREFIX := /usr/local
INSTALL := -m 755
BINARY := pimply

.PHONY: all clean install uninstall
all: $(BINARY)

include $(wildcard $(OBJDIR)/*.d)

$(BINARY): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(BINARY) $(LDFLAGS)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -MMD -c -o $@ $<

$(OBJDIR):
	mkdir $(@)

clean:
	rm -rf $(OBJDIR) *.d $(BINARY) *~

install:
	install $(INSTALL) $(BINARY) $(PREFIX)/bin

uninstall:
	rm $(PREFIX)/bin/$(BINARY)
