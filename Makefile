CC      = gcc
CFLAGS  = -g -I "$(shell pwd)"
RM      = rm -f

C_FILES   = $(wildcard */*.c)
O_FILES= $(C_FILES:.c=.o)


default: hcc

objs: $(O_FILES)

hcc: objs
	$(CC) $(CFLAGS) -o hcc $(O_FILES)

*/%.o : */%.c
	$(CC) $(CFLAGS) -c -o $@ $<

run: hcc
	./hcc

debug_run: run light_clean

clean: light_clean
	rm */*.o | true
light_clean:
	rm ./hcc | true
