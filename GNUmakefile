NAME=15puzzle

HAVE_GTK3:=$(shell pkg-config --modversion gtk+-3.0)

ifdef HAVE_GTK3
	ALL=$(NAME)-gtk3
	GTK = $(shell pkg-config --cflags gtk+-3.0) -DGTK3
	LDLIBS=`pkg-config --libs gtk+-3.0` 
else
	ALL=$(NAME)-gtk2
	GTK = $(shell pkg-config --cflags gtk+-2.0)
	LDLIBS=`pkg-config --libs gtk+-2.0`
endif

CC=gcc
CFLAGS=-std=c99 -g -O2 -Wall -DGTK_DISABLE_DEPRECATED=1 -DGDK_DISABLE_DEPRACATED=1 -DG_DISABLE_DEPRECATED=1 $(GTK)
LDFLAGS=-g


all: $(ALL)


$(NAME)-gtk2: $(NAME).o
	$(CC) $(LDFLAGS) -o $(NAME) $(NAME).o $(LDLIBS)
	
$(NAME)-gtk3: $(NAME).o
	$(CC) $(LDFLAGS) -o $(NAME) $(NAME).o $(LDLIBS)

clean:
	rm -f $(NAME).o $(NAME)
