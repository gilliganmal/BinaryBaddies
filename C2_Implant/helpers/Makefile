CC = CC=x86_64-w64-mingw32-gcc

# Compiler flags to enable all warnings & debug info
# CFLAGS = -Wall -Werror -g -O0
# CFLAGS += "-I./include"
# CFLAGS += "-lwinhttp"
# CFLAGS += "-DPB_ENABLE_MALLOC=1"

CSRCS := $(wildcard *.c)
OBJS := $(CSRCS:.c=.o)
TARGET = helpers

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

move:
	mv $(OBJS) "../obj"
clean:
	rm -f $(OBJS) $(TARGET)
