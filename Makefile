CC      = gcc
CFLAGS  = -g -I "$(shell pwd)"
RM      = rm -f

C_FILES     = $(wildcard */*.c)
O_FILES     = $(C_FILES:.c=.o)

TEST_C_FILES = $(wildcard _tests/*/*.c)
TEST_EXE    = $(TEST_C_FILES:.c=.out)

default: hcc

objs: $(O_FILES)

hcc: objs
	$(CC) $(CFLAGS) -o hcc $(O_FILES)

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

_tests/%.out : _tests/%.c
	$(CC) $(CFLAGS) -o $@ $<

tests: $(TEST_EXE)
	@for test in $(TEST_EXE); do \
		echo "Running $$test"; \
		./$$test; \
	done

run: hcc
	./hcc

debug_run: run light_clean

clean: light_clean
	$(RM) */*.o _tests/*.out _tests/*/*.out | true

light_clean:
	$(RM) ./hcc | true
