CC      = gcc
CFLAGS  = -I "$(shell pwd)"
DEBUG_FLAGS = -DDEBUG -g
RM      = rm -f

C_FILES     = $(wildcard parser/*.c)  $(wildcard common/*.c)
O_FILES     = $(C_FILES:.c=.o)

TEST_C_FILES = $(wildcard _tests/*/*.c)
TEST_EXE    = $(TEST_C_FILES:.c=.out)

INTERFACE_C_FILES = $(wildcard interface/*.c)
INTERFACE_O_FILES = $(INTERFACE_C_FILES:.c=.o)

default: hcc

objs: $(O_FILES)

hcc: objs $(INTERFACE_O_FILES)
	$(CC) $(CFLAGS) -o hcc $(O_FILES) $(INTERFACE_O_FILES)

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

_tests/%.out : _tests/%.c objs
	$(CC) $(CFLAGS) -o $@ $< $(O_FILES)

tests: CFLAGS += $(DEBUG_FLAGS)
tests: $(TEST_EXE)
	@for test in $(TEST_EXE); do \
		echo "Running $$test"; \
		./$$test; \
	done

run: hcc
	./hcc


debug_run: CFLAGS += $(DEBUG_FLAGS)
debug_run: run light_clean

clean: light_clean
	$(RM) */*.o _tests/*.out _tests/*/*.out | true

light_clean:
	$(RM) ./hcc | true
