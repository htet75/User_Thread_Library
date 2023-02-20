# Target library
lib := libuthread.a
CC := gcc
CFLAGS := -g -Wall -Werror
objs := context.o preempt.o queue.o sem.o uthread.o
deps := private.h

all: $(lib)

## TODO: Phase 1

$(lib): $(objs)
	ar rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< 

clean:
	rm -f $(lib) $(objs)