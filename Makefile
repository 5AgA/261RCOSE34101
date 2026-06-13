CC      = gcc
CFLAGS  = -Wall -Wextra -g -Iinclude

SRCS    = src/main.c       \
          src/runner.c     \
          src/process.c    \
          src/queue.c      \
          src/gantt.c      \
          src/evaluation.c \
          src/fcfs.c       \
          src/sjf.c        \
          src/priority.c \
          src/rr.c

TARGET  = scheduler

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
