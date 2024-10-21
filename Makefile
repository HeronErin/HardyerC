CC      = gcc
CFLAGS  = -g
RM      = rm -f

C_FILES   = $(wildcard src/*.c)
O_FILES= $(C_FILES:.c=.o)


default: hcc

objs: $(O_FILES)

hcc: objs
	gcc -o hcc $(O_FILES)



src/%.o : src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

run: hcc
	./hcc

debug_run: run clean

clean:
	rm ./hcc src/*.o

