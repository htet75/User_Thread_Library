# Target library
lib := libuthread.a
CC := gcc
CFLAGS := -g -Wall -Werror
objs := context.o preempt.o queue.o sem.o uthread.o
deps := private.h

all: $(lib)

## Debug flag
ifneq ($(D),1)
CFLAGS	+= -O2
else
CFLAGS	+= -g
endif

$(lib): $(objs)
	ar rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< 

clean:
	rm -f $(lib) $(objs)