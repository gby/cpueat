
CFLAGS = -Wall -g -D_GNU_SOURCE
LDFLAGS = -static

BIN = cpueat

all:  $(BIN)

clean:
	rm -f $(BIN) *.o *~
