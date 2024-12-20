TOPDIR  := ./
SRCDIR  := $(TOPDIR)src/
INCDIR  := $(TOPDIR)include/
OBJDIR  := $(TOPDIR)obj/
BINDIR  := $(TOPDIR)
NAME    := chat
EXE     := $(BINDIR)$(NAME)

SFILES  := cpp
OFILES  := o
CC      := gcc
CFLAGS  := -Wall -Wextra -O3 -pedantic -march=native -Wnull-dereference -Winline -Wconversion -g -fsanitize=address,undefined -I$(INCDIR)
LIBS    := -fsanitize=address,undefined -lstdc++

SOURCES := $(shell find $(SRCDIR) -name "*.$(SFILES)")
OBJECTS := $(patsubst $(SRCDIR)%.$(SFILES), $(OBJDIR)%.$(OFILES), $(SOURCES))

ALLFILES := $(SOURCES)

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJECTS)
	$(CC) $^ -o $@ $(LIBS)

$(OBJDIR)%$(OFILES): $(SRCDIR)%$(SFILES)
	$(CC) $(CFLAGS) $< -c -o $@

clean:
	@rm -f $(OBJECTS) $(EXE)
