GCC=gcc
CFLAGS=-std=c23 -D_DEFAULT_SOURCE
WARNFLAGS=-Wall -Wextra -Wshadow -Wformat=2 -Wnull-dereference -Wuninitialized -Wreturn-type -Wstrict-prototypes
IN=src
OUT=bin
DAYS=$(basename $(notdir $(wildcard $(IN)/day*.c)))
SOURCES=$(wildcard $(IN)/*.c)

day04: LDFLAGS=-lcrypto -lm
day12: LDFLAGS=-ljq

day%: init
	$(GCC) $(CFLAGS) $(WARNFLAGS) $(IN)/day$*.c -o $(OUT)/day$* $(LDFLAGS)

run%: day%
	$(OUT)/day$*

init:
	mkdir -p $(OUT)

clean:
	rm -f $(OUT)/day*

