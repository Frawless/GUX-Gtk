CC=gcc
CFLAGS=-std=c99 -g -O2 -Wall -DGTK_DISABLE_DEPRECATED=1 -DGDK_DISABLE_DEPRACATED=1 -DG_DISABLE_DEPRECATED=1 `pkg-config --cflags gtk+-3.0` -DGTK3
LDLIBS=`pkg-config --libs gtk+-3.0`
LDFLAGS=-g

NAME=15puzzle

$(NAME): $(NAME).o
	$(CC) $(LDFLAGS) -o $(NAME) $(NAME).o $(LDLIBS)

clean:
	rm -f $(NAME).o $(NAME)
