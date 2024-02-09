# Target library
lib := libuthread.a

CC := gcc
CFLAGS := -Wall -Wextra -Werror

# Source files
src := queue.c
obj := $(src:.c=.o)

all: $(lib)

$(lib): $(obj)
	ar rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(obj) $(lib)
