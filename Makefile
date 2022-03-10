BIN = trigger_data
SRCS = $(wildcard *.c)

CFLAGS += -Wall -g

$(BIN): $(SRCS)
	$(CC) $^ $(CFLAGS) -o $@
