SRC := $(wildcard *.c)
DEP := $(wildcard *.h) $(SRC)

sched: $(DEP)
	$(CC) -Wall -Wextra -ansi $(SRC) -o sched
