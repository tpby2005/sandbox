CC = gcc
CFLAGS = -ggdb -Wall -Wextra
LDFLAGS = -lraylib -lm

sandbox: main.c
	$(CC) $(CFLAGS) -o sandbox main.c $(LDFLAGS)

clean:
	rm -f sandbox

all: sandbox
