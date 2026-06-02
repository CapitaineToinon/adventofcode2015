GCC=gcc
CFLAGS=-std=c23 -D_DEFAULT_SOURCE
WARNFLAGS=-Wall -Wextra -Wshadow -Wformat=2 -Wnull-dereference -Wuninitialized -Wreturn-type -Wstrict-prototypes
DEBUGFLAGS=-fsanitize=address
IN=src
OUT=bin
DAYS=$(basename $(notdir $(wildcard $(IN)/day*.c)))
SOURCES=$(wildcard $(IN)/*.c)

COMMON_OBJ=$(OUT)/common.o

all: $(DAYS)

day04 run04: LDFLAGS=-lcrypto -lm
day20 run20: LDFLAGS=-lm

$(COMMON_OBJ): $(IN)/common.c $(IN)/common.h | init
	$(GCC) $(CFLAGS) $(WARNFLAGS) -c $(IN)/common.c -o $(COMMON_OBJ)

day%: init $(COMMON_OBJ)
	$(GCC) $(CFLAGS) $(WARNFLAGS) $(IN)/day$*.c $(COMMON_OBJ) -o $(OUT)/day$* $(LDFLAGS)

run%: init $(COMMON_OBJ)
	$(GCC) $(CFLAGS) $(WARNFLAGS) $(DEBUGFLAGS) $(IN)/day$*.c $(COMMON_OBJ) -o $(OUT)/run$* $(LDFLAGS)
	$(OUT)/run$*

run_all: all
	@for day in $(DAYS); do \
		echo "=== $$day ==="; \
		$(OUT)/$$day; \
	done

init:
	mkdir -p $(OUT)

clean:
	rm -f $(OUT)/day* $(OUT)/run* $(COMMON_OBJ)

