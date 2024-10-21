CC      = gcc
CFLAGS  = -g -I "$(shell pwd)"
RM      = rm -f

C_FILES     = $(wildcard parser/*.c)
O_FILES     = $(C_FILES:.c=.o)

TEST_C_FILES = $(wildcard _tests/*/*.c)
TEST_EXE    = $(TEST_C_FILES:.c=.out)

default: hcc

objs: $(O_FILES)

hcc: objs interface/*.o
	$(CC) $(CFLAGS) -o hcc $(O_FILES) interface/*.o

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

_tests/%.out : _tests/%.c objs
	$(CC) $(CFLAGS) -o $@ $< $(O_FILES)

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
