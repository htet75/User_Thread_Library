# Target library
lib := libuthread.a

CFLAGS := -Wall -Wextra -Werror
objs := queue.o

all: $(lib)

## TODO: Phase 1

$(lib): $(objs)
	ar rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(lib) $(objs)