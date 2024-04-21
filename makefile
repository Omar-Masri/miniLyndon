##
# Minimizer Demo
#
# @file
# @version 0.1

CC=gcc

compile:
	$(CC) `pkg-config --cflags glib-2.0` alg3.c `pkg-config --libs glib-2.0` -g -Ofast -o alg3.out

# end
