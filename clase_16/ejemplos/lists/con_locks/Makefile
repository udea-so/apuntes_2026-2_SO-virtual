# specify all source files here
SRCS = linked_list.c main.c
# specify target here (name of executable)
TARG = main.out
# specify compiler, compile flags, and needed libs
CC = gcc
OPTS = -Wall -O2 -g -pthread
LIBS = -pthread

# this translates .c files in src list to .o's
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARG)

# this generates the target executable
$(TARG): $(OBJS)
	$(CC) -o $(TARG) $(OBJS) $(LIBS)

# generic rule for .o files (also rebuilt when the header changes)
%.o: %.c linked_list.h
	$(CC) $(OPTS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARG)
