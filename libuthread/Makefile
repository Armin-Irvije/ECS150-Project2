# library
lib := libuthread.a

CC := gcc
CFLAGS := -Wall -Wextra -Werror

# Source files
# context.c preempt.c queue.c sem.c uthread.c
src := queue.c sem.c uthread.c context.c
obj := $(src:.c=.o)

all: $(lib)
#puts the object files into the library
$(lib): $(obj)
	ar rcs $@ $^ 

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
#clean all object files
clean:
	rm -f *~ $(obj) $(lib)
