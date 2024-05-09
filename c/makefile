##
# Minimizer_Demo
#
# @file
# @version 0.1

BINDIR = bin/

CC=gcc

CCP=clang++

OPTIMIZATION = -Ofast

CXXFLAGS = -m64 $(OPTIMIZATION)

CFLAGS = `pkg-config --cflags glib-2.0` $(OPTIMIZATION)

SRC_FILES_C = alg3.c utility.c fragments.c

OBJ_FILES_C = $(addprefix $(BINDIR), $(SRC_FILES_C:.c=.o))

HDR_FILES_C = $(SRC_FILES_C:.c=.h)

SRC_FILES_CP = fingerprint.cpp fingerprint_utils.cpp factorization_comb.cpp

OBJ_FILES_CP = $(addprefix $(BINDIR), $(SRC_FILES_CP:.cpp=.o))

HDR_FILES_CP = $(SRC_FILES_CP:.cpp=.h)

dir_guard=@mkdir -p $(@D)

all: $(BINDIR)minimizer_demo $(BINDIR)fingerprint

$(BINDIR)minimizer_demo: $(OBJ_FILES_C)
	$(dir_guard)
	$(CC) $(CFLAGS) $^ -o $@ `pkg-config --libs glib-2.0`

$(BINDIR)%.o: %.c $(HDR_FILES_C)
	$(dir_guard)
	$(CC) $(CFLAGS) -c -o $@ $< `pkg-config --libs glib-2.0`

$(BINDIR)fingerprint: $(OBJ_FILES_CP)
	$(dir_guard)
	$(CCP) $(CXXFLAGS) $^ -o $@

$(BINDIR)%.o: %.cpp $(HDR_FILES_CP)
	$(dir_guard)
	$(CCP) $(CXXFLAGS) -c -o $@ $<

.PHONY: clean doc

clean:
	rm -r $(BINDIR)

# end
