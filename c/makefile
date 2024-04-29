##
# Minimizer_Demo
#
# @file
# @version 0.1

CC=gcc

CFLAGS = `pkg-config --cflags glib-2.0` `pkg-config --libs glib-2.0` -g -Ofast

SRC_FILES = alg3.c utility.c

OBJ_FILES = $(SRC_FILES:.c=.o)

HDR_FILES = $(SRC_FILES:.c=.h)

all: minimizer_demo

minimizer_demo: $(OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

# end
