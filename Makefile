
CFLAGS = -Wall -g -D_GNU_SOURCE
LDFLAGS = 

BIN = cpueat

all:  $(BIN)

clean:
	rm -f $(BIN) *.o *~
